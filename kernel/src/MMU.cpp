#include <MMU.h>
struct MemoryBlock
{
    bool present;
    uint64_t physAddr;
    uint64_t numPages;
    uint64_t type;
    uint8_t* frames;
};
extern "C" char _kernel_start;
extern "C" char _kernel_end;
MemoryBlock blocks[64];
uint64_t kmallocStart, kmallocAddr;
PageDirectory* currentDirectory, *kernelDirectory;
bool mmuEnabled;
bool isUsable(uint32_t memoryType)
{
    switch (memoryType)
    {
    case EfiReservedMemoryType:
    case EfiLoaderCode:
    case EfiRuntimeServicesCode:
    case EfiRuntimeServicesData:
    case EfiUnusableMemory:
    case EfiACPIMemoryNVS:
    case EfiACPIReclaimMemory:
    case EfiMemoryMappedIO:
    case EfiMemoryMappedIOPortSpace:
    case EfiPalCode:
    case EfiMaxMemoryType:
        return false;
    default:
        return true;
    };
}
void reservePage(size_t addr)
{
    for (size_t i = 0; i < 64; i++)
    {
        if (!blocks[i].present) break;
        if (!isUsable(blocks[i].type)) continue;
        if (blocks[i].physAddr <= addr && (blocks[i].physAddr + blocks[i].numPages * 0x1000)
            > addr)
        {
            size_t index = (addr - blocks[i].physAddr) >> 12;
            blocks[i].frames[index / 8] |= 1 << index;
        }
    }
}
void freePage(size_t addr)
{
    for (size_t i = 0; i < 64; i++)
    {
        if (!blocks[i].present) break;
        if (!isUsable(blocks[i].type)) continue;
        if (blocks[i].physAddr <= addr && (blocks[i].physAddr + blocks[i].numPages * 0x1000)
            > addr)
        {
            size_t index = (addr - blocks[i].physAddr) >> 12;
            blocks[i].frames[index / 8] &= ~(1 << index);
        }
    }
}
uint64_t kmalloc(size_t size)
{
    uint64_t addr = kmallocAddr;
    kmallocAddr += size;
    return addr;
}
uint64_t kmalloc_a(size_t size)
{
    if (kmallocAddr & 0xFFF)
    {
        kmallocAddr &= ~0xFFF;
        kmallocAddr += 0x1000;
    }
    uint64_t addr = kmallocAddr;
    kmallocAddr += size;
    return addr;
}
uint64_t kmallocPage()
{
    for (size_t i = 0; i < 64; i++)
    {
        if (!blocks[i].present) break;
        if (!isUsable(blocks[i].type)) continue;
        for (size_t j = 0; j < blocks[i].numPages; j++)
        {
            if (!(blocks[i].frames[j / 8] & (1 << (j % 8))))
            {
                blocks[i].frames[j / 8] |= 1 << (j % 8);
                return blocks[i].physAddr + j * 0x1000;
            }
        }
    }
    return -1;
}
void switchDirectory(PageDirectory* dir)
{
    currentDirectory = dir;
    asm volatile ("mov %0, %%cr3":: "r"(dir));
}
PageDirectory::PageDirectory()
{
    memset(pml4, 0, sizeof(pml4));
}
void PageDirectory::mapPage(uint64_t virt, uint64_t phys, uint64_t flags)
{
    uint64_t pml4_index = (virt >> 39) & 0x1FF;
    uint64_t pdpt_index = (virt >> 30) & 0x1FF;
    uint64_t pd_index = (virt >> 21) & 0x1FF;
    uint64_t pt_index = (virt >> 12) & 0x1FF;
    if (!(pml4[pml4_index] & MMU_PRESENT))
    {
        uint64_t paddr = kmalloc_a(0x1000);
        memset((void*)paddr, 0, 0x1000);
        pml4[pml4_index] = paddr | flags;
    }
    uint64_t* pdpt = (uint64_t*)(pml4[pml4_index] & MMU_ADDR);
    if (!(pdpt[pdpt_index] & MMU_PRESENT))
    {
        uint64_t paddr = kmalloc_a(0x1000);
        memset((void*)paddr, 0, 0x1000);
        pdpt[pdpt_index] = paddr | flags;
    }
    uint64_t* pd = (uint64_t*)(pdpt[pdpt_index] & MMU_ADDR);
    if (!(pd[pd_index] & MMU_PRESENT))
    {
        uint64_t paddr = kmalloc_a(0x1000);
        memset((void*)paddr, 0, 0x1000);
        pd[pd_index] = paddr | flags;
    }
    uint64_t* pt = (uint64_t*)(pd[pd_index] & MMU_ADDR);
    pt[pt_index] = phys | flags;
    if (mmuEnabled && currentDirectory == this) switchDirectory(this);
}
void PageDirectory::map(uint64_t virt, uint64_t phys, size_t count, uint64_t flags)
{
    for (size_t i = 0; i < count; i++)
    {
        mapPage(virt + i * 0x1000, phys + i * 0x1000, flags);
    }
}
void initializeMMU(void* memoryMap, size_t memoryMapSize, size_t descriptorSize)
{
    mmuEnabled = false;
    for (size_t i = 0; i < 64; i++)
    {
        blocks[i].present = false;
    }
    size_t descriptorCount = memoryMapSize / descriptorSize;
    EFI_MEMORY_DESCRIPTOR* descriptor = (EFI_MEMORY_DESCRIPTOR*)memoryMap;
    size_t lowest = -1, highest = 0, largestStart, largestPages = 0;
    for (size_t i = 0; i < descriptorCount; i++)
    {
        if (descriptor->PhysicalStart < lowest) lowest = descriptor->PhysicalStart;
        if (descriptor->PhysicalStart + descriptor->NumberOfPages * EFI_PAGE_SIZE > highest)
            highest = descriptor->PhysicalStart + descriptor->NumberOfPages * EFI_PAGE_SIZE;
        if (descriptor->NumberOfPages > largestPages && isUsable(descriptor->Type))
        {
            largestStart = descriptor->PhysicalStart;
            largestPages = descriptor->NumberOfPages;
        }
        descriptor = NextMemoryDescriptor(descriptor, descriptorSize);
    }
    kmallocStart = largestStart;
    kmallocAddr = largestStart;
    descriptor = (EFI_MEMORY_DESCRIPTOR*)memoryMap;
    size_t k = 0;
    for (size_t i = 0; i < descriptorCount; i++)
    {
        if (k != 0 && descriptor->Type == blocks[k - 1].type
            && descriptor->PhysicalStart == blocks[k - 1].physAddr + blocks[k - 1].numPages
            * EFI_PAGE_SIZE)
        {
            blocks[k - 1].numPages += descriptor->NumberOfPages;
        }
        else
        {
            blocks[k].numPages = descriptor->NumberOfPages;
            blocks[k].physAddr = descriptor->PhysicalStart;
            blocks[k].type = descriptor->Type;
            blocks[k].present = true;
            k++;
        }
        descriptor = NextMemoryDescriptor(descriptor, descriptorSize);
    }
    for (size_t i = 0; i < 64; i++)
    {
        if (!blocks[i].present) break;
        if (isUsable(blocks[i].type))
        {
            blocks[i].frames = (uint8_t*)kmalloc((blocks[i].numPages + 7) / 8);
            memset(blocks[i].frames, 0, (blocks[i].numPages + 7) / 8);
        }
        else blocks[i].frames = NULL;
    }
    kernelDirectory = (PageDirectory*)kmalloc_a(sizeof(PageDirectory));
    *kernelDirectory = PageDirectory();
    for (size_t i = 0; i < 64; i++)
    {
        if (!blocks[i].present) break;
        kernelDirectory->map(blocks[i].physAddr, blocks[i].physAddr, blocks[i].numPages,
            MMU_PRESENT | MMU_RW);
    }
    size_t kernel_start = (size_t)&_kernel_start;
    size_t kernel_end = (size_t)&_kernel_end;
    for (size_t i = kernel_start; i < kernel_end; i += 0x1000)
    {
        reservePage(i);
    }
    switchDirectory(kernelDirectory);
}
PageDirectory* PageDirectory::clone()
{
    PageDirectory* directory = (PageDirectory*)kmalloc_a(sizeof(PageDirectory));
    for (size_t i = 0; i < 512; i++)
    {
        if (pml4[i] & MMU_PRESENT)
        {
            uint64_t* pdpt = (uint64_t*)(pml4[i] & MMU_ADDR);
            uint64_t pdptPhys = kmalloc_a(0x1000);
            directory->pml4[i] = pdptPhys | MMU_RW | MMU_PRESENT;
            uint64_t* pdptNew = (uint64_t*)(directory->pml4[i] & MMU_ADDR);
            for (size_t j = 0; j < 512; j++)
            {
                if (pdpt[j] & MMU_PRESENT)
                {
                    uint64_t* pd = (uint64_t*)(pdpt[j] & MMU_ADDR);
                    uint64_t pdPhys = kmalloc_a(0x1000);
                    pdptNew[j] = pdPhys | MMU_RW | MMU_PRESENT;
                    uint64_t* pdNew = (uint64_t*)(pdptNew[j] & MMU_ADDR);
                    for (size_t k = 0; k < 512; k++)
                    {
                        if (pd[k] & MMU_PRESENT)
                        {
                            uint64_t* pt = (uint64_t*)(pd[k] & MMU_ADDR);
                            uint64_t ptPhys = kmalloc_a(0x1000);
                            pdNew[k] = ptPhys | MMU_RW | MMU_PRESENT;
                            uint64_t* ptNew = (uint64_t*)(pdNew[k] & MMU_ADDR);
                            memcpy(ptNew, pt, 0x1000);
                        }
                        else
                        {
                            pdNew[k] = 0;
                        }
                    }
                }
                else
                {
                    pdptNew[j] = 0;
                }
            }
        }
        else
        {
            directory->pml4[i] = 0;
        }
    }
    return directory;
}