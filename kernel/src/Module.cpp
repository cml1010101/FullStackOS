#include <Module.h>
#include <elf.h>
Vector<Module> modules;
extern PageDirectory* kernelDirectory;
Module::Module(const void* data, size_t len)
{
    ModuleHeader* header = (ModuleHeader*)data;
    if (header->magic == 0xBEEFCAFE)
    {
        memcpy(moduleName, header->moduleName, 16);
        moduleName[16] = 0;
        moduleType = header->moduleType;
        directory = kernelDirectory->clone();
        switchDirectory(directory);
        dataVirt = header->dataLoadAddress;
        dataLength = header->dataSize;
        dataPhys = (uint64_t*)kmalloc(((dataLength + 0xFFF) / 0x1000) * sizeof(uint64_t));
        for (size_t i = 0; i < dataLength; i += 0x1000)
        {
            dataPhys[i] = kmallocPage();
            directory->mapPage(dataVirt + i, dataPhys[i], MMU_RW | MMU_PRESENT);
            memcpy((void*)dataVirt, &header->data[header->dataOffset], header->dataSize);
        }
        Elf64_Ehdr* elfHeader = (Elf64_Ehdr*)&header->data[header->elfOffset];
        codeVirt = -1;
        codeLength = 0;
        size_t numPages = 0;
        for (size_t i = 0; i < elfHeader->e_phnum; i++)
        {
            Elf64_Phdr* phdr = (Elf64_Phdr*)&header->data[header->elfOffset + elfHeader->e_phoff];
            if (phdr->p_type == PT_LOAD)
            {
                if (phdr->p_vaddr < codeVirt)
                {
                    codeVirt = phdr->p_vaddr;
                }
                codeLength += phdr->p_filesz;
                numPages += (phdr->p_filesz + 0xFFF) / 0x1000;
            }
        }
        codePhys = new uint64_t[numPages];
        for (size_t i = 0; i < elfHeader->e_phnum; i++)
        {
            Elf64_Phdr* phdr = (Elf64_Phdr*)&header->data[header->elfOffset + elfHeader->e_phoff
                + i * elfHeader->e_phentsize];
            if (phdr->p_type == PT_LOAD)
            {
                for (size_t i = 0; i < phdr->p_filesz; i += 0x1000)
                {
                    codePhys[i] = kmallocPage();
                    directory->mapPage(codeVirt + i, dataPhys[i], MMU_RW | MMU_PRESENT);
                    memcpy((void*)dataVirt, &header->data[header->dataOffset], header->dataSize);
                }
            }
        }
        for (size_t i = 0; i < elfHeader->e_shnum; i++)
        {
            Elf64_Shdr* shdr = (Elf64_Shdr*)&header->data[header->elfOffset + elfHeader->e_shoff
                + i * elfHeader->e_shentsize];
            if (shdr->sh_type == SHT_SYMTAB)
            {
                Elf64_Shdr* strtab = (Elf64_Shdr*)&header->data[header->elfOffset
                    + elfHeader->e_shoff + shdr->sh_link * elfHeader->e_shentsize];
            }
        }
        initializationEntry = (void(*)())elfHeader->e_entry;
        switchDirectory(kernelDirectory);
        initThread = new Thread(initializationEntry, moduleName);
    }
}
void Module::initialize()
{
    addThread(initThread);
    join(initThread->pid);
}
void Module::summarize()
{
    qemu_printf("[%s]: %d bytes of code; %d bytes of data\n");
}