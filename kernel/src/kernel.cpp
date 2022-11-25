#include <SmartOS.h>
#include <QEMU.h>
#include <CPU.h>
#include <Heap.h>
extern "C" void kernel_main(BootData data)
{
    asm volatile ("cli");
    qemu = QEMUDriver();
    cpu = CPU(data.memoryMap, data.mapSize, data.descriptorSize);
    const char* vendorStr = cpu.getVendor();
    qemu.printf("Found CPU: %s\n", vendorStr);
    cpu.setupGDT();
    cpu.setupIDT();
    qemu.printf("Loaded GDT & IDT!\n");
    cpu.mmu.summarize();
    cpu.setupPaging();
    qemu.printf("Setup Paging\n");
    qemu.printf("Does this operating system support APIC? ");
    qemu.printf("%s\n", cpu.supportAPIC() ? "Yes" : "No");
    mmu->currentDirectory->map(0x50000, 0xC000000, 1, MMU_RW);
    *(uint32_t*)0x50000 = 0xDEADBEEF;
    qemu.printf("0xC000000 = 0x%x\n", *(uint64_t*)0xC000000);
    asm volatile("sti");
    for (;;);
}