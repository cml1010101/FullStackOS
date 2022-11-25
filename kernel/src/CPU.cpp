#include <CPU.h>
#include <QEMU.h>
#define CPUID_VENDOR_AMD           "AuthenticAMD"
#define CPUID_VENDOR_AMD_OLD       "AMDisbetter!"
#define CPUID_VENDOR_INTEL         "GenuineIntel"
#define CPUID_VENDOR_VIA           "VIA VIA VIA "
#define CPUID_VENDOR_TRANSMETA     "GenuineTMx86"
#define CPUID_VENDOR_TRANSMETA_OLD "TransmetaCPU"
#define CPUID_VENDOR_CYRIX         "CyrixInstead"
#define CPUID_VENDOR_CENTAUR       "CentaurHauls"
#define CPUID_VENDOR_NEXGEN        "NexGenDriven"
#define CPUID_VENDOR_UMC           "UMC UMC UMC "
#define CPUID_VENDOR_SIS           "SiS SiS SiS "
#define CPUID_VENDOR_NSC           "Geode by NSC"
#define CPUID_VENDOR_RISE          "RiseRiseRise"
#define CPUID_VENDOR_VORTEX        "Vortex86 SoC"
#define CPUID_VENDOR_AO486         "MiSTer AO486"
#define CPUID_VENDOR_AO486_OLD     "GenuineAO486"
#define CPUID_VENDOR_ZHAOXIN       "  Shanghai  "
#define CPUID_VENDOR_HYGON         "HygonGenuine"
#define CPUID_VENDOR_ELBRUS        "E2K MACHINE "
#define CPUID_VENDOR_QEMU          "TCGTCGTCGTCG"
#define CPUID_VENDOR_KVM           " KVMKVMKVM  "
#define CPUID_VENDOR_VMWARE        "VMwareVMware"
#define CPUID_VENDOR_VIRTUALBOX    "VBoxVBoxVBox"
#define CPUID_VENDOR_XEN           "XenVMMXenVMM"
#define CPUID_VENDOR_HYPERV        "Microsoft Hv"
#define CPUID_VENDOR_PARALLELS     " prl hyperv "
#define CPUID_VENDOR_PARALLELS_ALT " lrpepyh vr "
#define CPUID_VENDOR_BHYVE         "bhyve bhyve "
#define CPUID_VENDOR_QNX           " QNXQVMBSQG "
enum {
    CPUID_FEAT_ECX_SSE3         = 1 << 0, 
    CPUID_FEAT_ECX_PCLMUL       = 1 << 1,
    CPUID_FEAT_ECX_DTES64       = 1 << 2,
    CPUID_FEAT_ECX_MONITOR      = 1 << 3,  
    CPUID_FEAT_ECX_DS_CPL       = 1 << 4,  
    CPUID_FEAT_ECX_VMX          = 1 << 5,  
    CPUID_FEAT_ECX_SMX          = 1 << 6,  
    CPUID_FEAT_ECX_EST          = 1 << 7,  
    CPUID_FEAT_ECX_TM2          = 1 << 8,  
    CPUID_FEAT_ECX_SSSE3        = 1 << 9,  
    CPUID_FEAT_ECX_CID          = 1 << 10,
    CPUID_FEAT_ECX_SDBG         = 1 << 11,
    CPUID_FEAT_ECX_FMA          = 1 << 12,
    CPUID_FEAT_ECX_CX16         = 1 << 13, 
    CPUID_FEAT_ECX_XTPR         = 1 << 14, 
    CPUID_FEAT_ECX_PDCM         = 1 << 15, 
    CPUID_FEAT_ECX_PCID         = 1 << 17, 
    CPUID_FEAT_ECX_DCA          = 1 << 18, 
    CPUID_FEAT_ECX_SSE4_1       = 1 << 19, 
    CPUID_FEAT_ECX_SSE4_2       = 1 << 20, 
    CPUID_FEAT_ECX_X2APIC       = 1 << 21, 
    CPUID_FEAT_ECX_MOVBE        = 1 << 22, 
    CPUID_FEAT_ECX_POPCNT       = 1 << 23, 
    CPUID_FEAT_ECX_TSC          = 1 << 24, 
    CPUID_FEAT_ECX_AES          = 1 << 25, 
    CPUID_FEAT_ECX_XSAVE        = 1 << 26, 
    CPUID_FEAT_ECX_OSXSAVE      = 1 << 27, 
    CPUID_FEAT_ECX_AVX          = 1 << 28,
    CPUID_FEAT_ECX_F16C         = 1 << 29,
    CPUID_FEAT_ECX_RDRAND       = 1 << 30,
    CPUID_FEAT_ECX_HYPERVISOR   = 1 << 31,
    CPUID_FEAT_EDX_FPU          = 1 << 0,  
    CPUID_FEAT_EDX_VME          = 1 << 1,  
    CPUID_FEAT_EDX_DE           = 1 << 2,  
    CPUID_FEAT_EDX_PSE          = 1 << 3,  
    CPUID_FEAT_EDX_TSC          = 1 << 4,  
    CPUID_FEAT_EDX_MSR          = 1 << 5,  
    CPUID_FEAT_EDX_PAE          = 1 << 6,  
    CPUID_FEAT_EDX_MCE          = 1 << 7,  
    CPUID_FEAT_EDX_CX8          = 1 << 8,  
    CPUID_FEAT_EDX_APIC         = 1 << 9,  
    CPUID_FEAT_EDX_SEP          = 1 << 11, 
    CPUID_FEAT_EDX_MTRR         = 1 << 12, 
    CPUID_FEAT_EDX_PGE          = 1 << 13, 
    CPUID_FEAT_EDX_MCA          = 1 << 14, 
    CPUID_FEAT_EDX_CMOV         = 1 << 15, 
    CPUID_FEAT_EDX_PAT          = 1 << 16, 
    CPUID_FEAT_EDX_PSE36        = 1 << 17, 
    CPUID_FEAT_EDX_PSN          = 1 << 18, 
    CPUID_FEAT_EDX_CLFLUSH      = 1 << 19, 
    CPUID_FEAT_EDX_DS           = 1 << 21, 
    CPUID_FEAT_EDX_ACPI         = 1 << 22, 
    CPUID_FEAT_EDX_MMX          = 1 << 23, 
    CPUID_FEAT_EDX_FXSR         = 1 << 24, 
    CPUID_FEAT_EDX_SSE          = 1 << 25, 
    CPUID_FEAT_EDX_SSE2         = 1 << 26, 
    CPUID_FEAT_EDX_SS           = 1 << 27, 
    CPUID_FEAT_EDX_HTT          = 1 << 28, 
    CPUID_FEAT_EDX_TM           = 1 << 29, 
    CPUID_FEAT_EDX_IA64         = 1 << 30,
    CPUID_FEAT_EDX_PBE          = 1 << 31
};
#define cpuid(in, a, b, c, d) asm("cpuid": "=a" (a), "=b" (b), "=c" (c), "=d" (d) : "a" (in))
extern "C" size_t check_cpuid();
extern "C" void load_gdt(SystemPointer* ptr, size_t cs, size_t ds, size_t es, size_t fs, size_t gs,
    size_t ss);
char vendorString[13];
CPU cpu;
const char* MEMORY_TYPE_TO_STRING[] = {
    "EfiReservedMemoryType",
    "EfiLoaderCode",
    "EfiLoaderData",
    "EfiBootServicesCode",
    "EfiBootServicesData",
    "EfiRuntimeServicesCode",
    "EfiRuntimeServicesData",
    "EfiConventionalMemory",
    "EfiUnusableMemory",
    "EfiACPIReclaimMemory",
    "EfiACPIMemoryNVS",
    "EfiMemoryMappedIO",
    "EfiMemoryMappedIOPortSpace",
    "EfiPalCode",
    "EfiPersistentMemory",
    "EfiMaxMemoryType"
};
PageDirectory::PageDirectory()
{
    memset(pml4, 0, sizeof(pml4) + 8);
}
MMU* mmu;
void PageDirectory::mapPage(size_t phys, size_t virt, uint64_t flags)
{
    uint64_t pml4Index = (virt >> 39) & 0x1FF;
    uint64_t pdptIndex = (virt >> 30) & 0x1FF;
    uint64_t pdIndex = (virt >> 21) & 0x1FF;
    uint64_t ptIndex = (virt >> 12) & 0x1FF;
    if (!pml4[pml4Index])
    {
        uint64_t paddr = mmu->kmalloc(4096);
        memset((void*)paddr, 0, 4096);
        pml4[pml4Index] = paddr | flags | MMU_PRESENT;
    }
    uint64_t* pdpt = (uint64_t*)(pml4[pml4Index] & MMU_ADDR);
    if (!pdpt[pdptIndex])
    {
        uint64_t paddr = mmu->kmalloc(4096);
        memset((void*)paddr, 0, 4096);
        pdpt[pdptIndex] = paddr | flags | MMU_PRESENT;
    }
    uint64_t* pd = (uint64_t*)(pdpt[pdptIndex] & MMU_ADDR);
    if (!pd[pdIndex])
    {
        uint64_t paddr = mmu->kmalloc(4096);
        memset((void*)paddr, 0, 4096);
        pd[pdIndex] = paddr | flags | MMU_PRESENT;
    }
    uint64_t* pt = (uint64_t*)(pd[pdIndex] & MMU_ADDR);
    pt[ptIndex] = phys | flags | MMU_PRESENT;
    if (mmu->currentDirectory == this) mmu->setDirectory(this);
}
void PageDirectory::map(size_t phys, size_t virt, size_t pages, uint64_t flags)
{
    for (size_t i = 0; i < pages; i++)
    {
        mapPage(phys + (i * 0x1000), virt + (i * 0x1000), flags);
    }
}
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
MMU::MMU(void* memoryMap, size_t memoryMapSize, size_t descriptorSize)
    : memoryMap(memoryMap), memoryMapSize(memoryMapSize), descriptorSize(descriptorSize)
{
    currentDirectory = NULL;
    kernelDirectory = NULL;
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
    descriptor = (EFI_MEMORY_DESCRIPTOR*)memoryMap;
    setFrames(largestStart / 0x1000, largestPages);
    size_t k = 0;
    for (size_t i = 0; i < descriptorCount; i++)
    {
        if (!isUsable(descriptor->Type))
        {
            setFrames(descriptor->PhysicalStart / EFI_PAGE_SIZE, descriptor->NumberOfPages);
        }
        if (k != 0 && descriptor->Type == blocks[k - 1].type
            && descriptor->PhysicalStart == blocks[k - 1].physStart + blocks[k - 1].numPages
            * EFI_PAGE_SIZE)
        {
            blocks[k - 1].numPages += descriptor->NumberOfPages;
        }
        else
        {
            blocks[k].numPages = descriptor->NumberOfPages;
            blocks[k].physStart = descriptor->PhysicalStart;
            blocks[k].type = descriptor->Type;
            blocks[k].present = true;
            k++;
        }
        descriptor = NextMemoryDescriptor(descriptor, descriptorSize);
    }
}
void MMU::setDirectory(PageDirectory* dir)
{
    currentDirectory = dir;
    asm volatile ("mov %0, %%cr3":: "r"(dir->phys));
}
void CPU::setupPaging()
{
    mmu.kernelDirectory = (PageDirectory*)mmu.kmalloc(sizeof(PageDirectory));
    *mmu.kernelDirectory = PageDirectory();
    mmu.kernelDirectory->phys = (uint64_t)mmu.kernelDirectory;
    for (size_t i = 0; i < 64; i++)
    {
        if (!mmu.blocks[i].present) break;
        mmu.kernelDirectory->map(mmu.blocks[i].physStart, mmu.blocks[i].physStart,
            mmu.blocks[i].numPages, MMU_PRESENT | MMU_RW);
    }
    mmu.setDirectory(mmu.kernelDirectory);
}
uint64_t MMU::kmalloc(size_t size, bool align)
{
    if (align && kmallocStart & 0xFFF)
    {
        kmallocStart &= 0xFFF;
        kmallocStart += 0x1000;
    }
    size_t addr = kmallocStart;
    kmallocStart += size;
    return addr;
}
uint8_t MMU::getFrame(size_t i)
{
    return (frames[i / 8] >> (i % 8)) & 1;
}
void MMU::setFrame(size_t i)
{
    frames[i / 8] |= 1 << (i % 8);
}
void MMU::clearFrame(size_t i)
{
    frames[i / 8] &= ~(1 << (i % 8));
}
void MMU::setFrames(size_t index, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        setFrame(index + i);
    }
}
size_t MMU::allocFrame()
{
    size_t i;
    for (i = 0; i < maxFrames; i++)
    {
        if ((frames[i / 8] >> (i % 8) ) & 1 == 0) break;
    }
    setFrame(i);
    return i;
}
void MMU::summarize()
{
    for (size_t i = 0; i < 64; i++)
    {
        if (!blocks[i].present) break;
        qemu_printf("[0x%x-0x%x]: %s\n", blocks[i].physStart,
            blocks[i].physStart + blocks[i].numPages * EFI_PAGE_SIZE,
            MEMORY_TYPE_TO_STRING[blocks[i].type]);
    }
}
CPU::CPU(void* memoryMap, size_t memoryMapSize, size_t descriptorSize)
{
    this->mmu = MMU(memoryMap, memoryMapSize, descriptorSize);
    ::mmu = &this->mmu;
    initialized = false;
}
bool CPU::supportsCPUID()
{
    return check_cpuid();
}
const char* CPU::getVendor()
{
    uint32_t eax, ebx, ecx, edx;
    cpuid(0, eax, ebx, ecx, edx);
	vendorString[13] = '\0';
	for(size_t i = 0; i < 4; i++)
    {
		vendorString[i] = ebx >> (8 * i);
		vendorString[i + 4] = edx >> (8 * i);
		vendorString[i + 8] = ecx >> (8 * i);
	}
    return vendorString;
}
const char* CPU::getName()
{
    return getVendor();
}
void CPU::loadFeatures()
{
    uint32_t rsv0, rsv1;
    cpuid(0, rsv0, rsv1, featuresECX, featuresEDX);
}
GDTEntry::GDTEntry()
{
    limit0 = 0;
    base0 = 0;
    base1 = 0;
    access = 0;
    limit1 = 0;
    flags = 0;
    base2 = 0;
}
GDTEntry::GDTEntry(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
    limit0 = limit & 0xFFFF;
    base0 = base & 0xFFFF;
    base1 = (base >> 16) & 0xFF;
    this->access = access;
    limit1 = (limit >> 16) & 0xF;
    this->flags = flags;
    base2 = (base >> 24) & 0xFF;
}
GDT::GDT()
{
    entries[1] = GDTEntry(0, 0xFFFFF, 0x9A, 0xA);
    entries[2] = GDTEntry(0, 0xFFFFF, 0x92, 0xC);
    address.base = (uint64_t)&entries[0];
    address.limit = sizeof(entries) - 1;
}
void GDT::load(size_t cs, size_t ds, size_t es, size_t fs, size_t gs, size_t ss)
{
    address.base = (uint64_t)&entries[0];
    address.limit = sizeof(entries) - 1;
    load_gdt(&address, cs, ds, es, fs, gs, ss);
}
GDTEntry GDT::operator[](size_t i) const
{
    return entries[i];
}
GDTEntry& GDT::operator[](size_t i)
{
    return entries[i];
}
void CPU::setupGDT()
{
    gdt = GDT();
    gdt.load(0x8, 0x10, 0x10, 0x10, 0x10, 0x10);
}
IDTEntry::IDTEntry()
{
    offset0 = 0;
    selector = 0;
    ist = 0;
    rsv0 = 0;
    type = 0;
    dpl = 0;
    p = 0;
    offset1 = 0;
    offset2 = 0;
    rsv1 = 0;
}
IDTEntry::IDTEntry(uint64_t offset, uint16_t selector, uint8_t type, uint8_t dpl, uint8_t ist)
{
    offset0 = offset & 0xFFFF;
    this->selector = selector;
    this->ist = ist;
    rsv0 = 0;
    this->type = type;
    this->dpl = dpl;
    p = 1;
    offset1 = (offset >> 16) & 0xFFFF;
    offset2 = (offset >> 32) & 0xFFFFFFFF;
    rsv1 = 0;
}
extern "C" void isr0();
extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();
extern "C" void isr5();
extern "C" void isr6();
extern "C" void isr7();
extern "C" void isr8();
extern "C" void isr9();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();
extern "C" void isr13();
extern "C" void isr14();
extern "C" void isr15();
extern "C" void isr16();
extern "C" void isr17();
extern "C" void isr18();
extern "C" void isr19();
extern "C" void isr20();
extern "C" void isr21();
extern "C" void isr22();
extern "C" void isr23();
extern "C" void isr24();
extern "C" void isr25();
extern "C" void isr26();
extern "C" void isr27();
extern "C" void isr28();
extern "C" void isr29();
extern "C" void isr30();
extern "C" void isr31();
extern "C" void irq0();
extern "C" void irq1();
extern "C" void irq2();
extern "C" void irq3();
extern "C" void irq4();
extern "C" void irq5();
extern "C" void irq6();
extern "C" void irq7();
extern "C" void irq8();
extern "C" void irq9();
extern "C" void irq10();
extern "C" void irq11();
extern "C" void irq12();
extern "C" void irq13();
extern "C" void irq14();
extern "C" void irq15();
IDT::IDT()
{
    entries[0] = IDTEntry((uint64_t)isr0, 0x8, 0b1110, 0, 0);
    entries[1] = IDTEntry((uint64_t)isr1, 0x8, 0b1110, 0, 0);
    entries[2] = IDTEntry((uint64_t)isr2, 0x8, 0b1110, 0, 0);
    entries[3] = IDTEntry((uint64_t)isr3, 0x8, 0b1110, 0, 0);
    entries[4] = IDTEntry((uint64_t)isr4, 0x8, 0b1110, 0, 0);
    entries[5] = IDTEntry((uint64_t)isr5, 0x8, 0b1110, 0, 0);
    entries[6] = IDTEntry((uint64_t)isr6, 0x8, 0b1110, 0, 0);
    entries[7] = IDTEntry((uint64_t)isr7, 0x8, 0b1110, 0, 0);
    entries[8] = IDTEntry((uint64_t)isr8, 0x8, 0b1110, 0, 0);
    entries[9] = IDTEntry((uint64_t)isr9, 0x8, 0b1110, 0, 0);
    entries[10] = IDTEntry((uint64_t)isr10, 0x8, 0b1110, 0, 0);
    entries[11] = IDTEntry((uint64_t)isr11, 0x8, 0b1110, 0, 0);
    entries[12] = IDTEntry((uint64_t)isr12, 0x8, 0b1110, 0, 0);
    entries[13] = IDTEntry((uint64_t)isr13, 0x8, 0b1110, 0, 0);
    entries[14] = IDTEntry((uint64_t)isr14, 0x8, 0b1110, 0, 0);
    entries[15] = IDTEntry((uint64_t)isr15, 0x8, 0b1110, 0, 0);
    entries[16] = IDTEntry((uint64_t)isr16, 0x8, 0b1110, 0, 0);
    entries[17] = IDTEntry((uint64_t)isr17, 0x8, 0b1110, 0, 0);
    entries[18] = IDTEntry((uint64_t)isr18, 0x8, 0b1110, 0, 0);
    entries[19] = IDTEntry((uint64_t)isr19, 0x8, 0b1110, 0, 0);
    entries[20] = IDTEntry((uint64_t)isr20, 0x8, 0b1110, 0, 0);
    entries[21] = IDTEntry((uint64_t)isr21, 0x8, 0b1110, 0, 0);
    entries[22] = IDTEntry((uint64_t)isr22, 0x8, 0b1110, 0, 0);
    entries[23] = IDTEntry((uint64_t)isr23, 0x8, 0b1110, 0, 0);
    entries[24] = IDTEntry((uint64_t)isr24, 0x8, 0b1110, 0, 0);
    entries[25] = IDTEntry((uint64_t)isr25, 0x8, 0b1110, 0, 0);
    entries[26] = IDTEntry((uint64_t)isr26, 0x8, 0b1110, 0, 0);
    entries[27] = IDTEntry((uint64_t)isr27, 0x8, 0b1110, 0, 0);
    entries[28] = IDTEntry((uint64_t)isr28, 0x8, 0b1110, 0, 0);
    entries[29] = IDTEntry((uint64_t)isr29, 0x8, 0b1110, 0, 0);
    entries[30] = IDTEntry((uint64_t)isr30, 0x8, 0b1110, 0, 0);
    entries[31] = IDTEntry((uint64_t)isr31, 0x8, 0b1110, 0, 0);
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
    outb(0xA1, 0xFF);
    outb(0x21, 0xFF);
    entries[32] = IDTEntry((uint64_t)irq0, 0x8, 0b1110, 0, 0);
    entries[33] = IDTEntry((uint64_t)irq1, 0x8, 0b1110, 0, 0);
    entries[34] = IDTEntry((uint64_t)irq2, 0x8, 0b1110, 0, 0);
    entries[35] = IDTEntry((uint64_t)irq3, 0x8, 0b1110, 0, 0);
    entries[36] = IDTEntry((uint64_t)irq4, 0x8, 0b1110, 0, 0);
    entries[37] = IDTEntry((uint64_t)irq5, 0x8, 0b1110, 0, 0);
    entries[38] = IDTEntry((uint64_t)irq6, 0x8, 0b1110, 0, 0);
    entries[39] = IDTEntry((uint64_t)irq7, 0x8, 0b1110, 0, 0);
    entries[40] = IDTEntry((uint64_t)irq8, 0x8, 0b1110, 0, 0);
    entries[41] = IDTEntry((uint64_t)irq9, 0x8, 0b1110, 0, 0);
    entries[42] = IDTEntry((uint64_t)irq10, 0x8, 0b1110, 0, 0);
    entries[43] = IDTEntry((uint64_t)irq11, 0x8, 0b1110, 0, 0);
    entries[44] = IDTEntry((uint64_t)irq12, 0x8, 0b1110, 0, 0);
    entries[45] = IDTEntry((uint64_t)irq13, 0x8, 0b1110, 0, 0);
    entries[46] = IDTEntry((uint64_t)irq14, 0x8, 0b1110, 0, 0);
    entries[47] = IDTEntry((uint64_t)irq15, 0x8, 0b1110, 0, 0);
}
IDTEntry IDT::operator[](size_t i) const
{
    return entries[i];
}
IDTEntry& IDT::operator[](size_t i)
{
    return entries[i];
}
extern "C" void load_idt(SystemPointer* ptr);
void IDT::load()
{
    address.base = (uint64_t)&entries[0];
    address.limit = sizeof(entries) - 1;
    load_idt(&address);
}
char* isr_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};
void CPU::setupIDT()
{
    for (size_t i = 0; i < 16; i++)
    {
        irqHandlers[i] = (IRQHandler)0;
    }
    idt = IDT();
    idt.load();
}
extern "C" void isr_handler(CPURegisters* regs)
{
    qemu_printf("[0x%x]: %s(0x%x)\n", regs->rip, isr_messages[regs->num], regs->code);
    for (;;);
}
extern "C" void irq_handler(CPURegisters* regs)
{
    if (cpu.irqHandlers[regs->num]) cpu.irqHandlers[regs->num](regs);
    cpu.apic[APIC_EOI] = 0;
}
void CPU::writeMSR(uint32_t reg, uint32_t eax, uint32_t edx)
{
    asm volatile ("wrmsr":: "a"(eax), "d"(edx), "c"(reg));
}
void CPU::readMSR(uint32_t reg, uint32_t& eax, uint32_t& edx)
{
    asm volatile ("rdmsr": "=a"(eax), "=d"(edx): "c"(reg));
}
bool CPU::supportAPIC()
{
    if (!initialized)
    {
        loadFeatures();
    }
    return CPUID_FEAT_EDX_APIC & featuresEDX;
}
void CPU::setAPICBase(uint64_t base)
{
    uint32_t eax = (base & 0xFFFFF000) | 0x800;
    uint32_t edx = (base >> 32) & 0xF;
    writeMSR(0x1B, eax, edx);
}
uint64_t CPU::getAPICBase()
{
    uint32_t eax, edx;
    readMSR(0x1B, eax, edx);
    return (uint64_t)eax | (((uint64_t)edx << 32) & 0xF);
}
void CPU::enableAPIC()
{
    setAPICBase((uint64_t)(apic = (uint32_t*)getAPICBase()));
    mmu.currentDirectory->mapPage((uint64_t)apic, (uint64_t)apic, MMU_RW | MMU_PRESENT | MMU_NO_CACHE);
    apic[APIC_SIVR] |= 0x100;
    apic[APIC_DFR] = 0xFFFFFFFF;
    uint32_t ldr = apic[APIC_LDR];
    ldr = (ldr & 0xFFFFFF) | 1;
    apic[APIC_LDR] = ldr;
    apic[APIC_TIMER] = 0x10000;
    apic[APIC_PMCR] = 4 << 8;
    apic[APIC_LINT0] = 0x10000;
    apic[APIC_LINT1] = 0x10000;
    apic[APIC_TPR] = 0;
    apic[APIC_SIVR] = 0x100 | 39;
    apic[APIC_TIMER] = 32;
    apic[APIC_DCOUNT] = 0x3;
    uint8_t tmp = inb(0x61);
    tmp = (tmp & 0xFD) | 1;
    outb(0x61, tmp);
    outb(0x43, 0b10110010);
    outb(0x42, 0x9B);
    inb(0x60);
    outb(0x42, 0x2E);
    tmp = inb(0x61);
    tmp &= 0xFE;
    outb(0x61, tmp);
    tmp |= 1;
    outb(0x61, tmp);
    apic[APIC_ICOUNT] = 0xFFFFFFFF;
    while (inb(0x61) & 0x20);
    apic[APIC_TIMER] = 0x10000;
    uint32_t timerval = apic[APIC_CCOUNT];
    uint32_t diff = (0xFFFFFFFF - timerval) + 1;
    diff *= 16;
    diff *= 100;
    diff /= 1000;
    diff /= 16;
    apic[APIC_ICOUNT] = diff;
    apic[APIC_TIMER] = 32 | 0x20000;
    apic[APIC_DCOUNT] = 0x3;
}