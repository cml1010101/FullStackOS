#include <SmartOS.h>
#include <GDT.h>
#include <IDT.h>
#include <IRQ.h>
#include <PIT.h>
#include <MMU.h>
#include <Heap.h>
#include <Scheduler.h>
#include <Graphics.h>
#include <PCI.h>
#include <IDE.h>
struct BootData
{
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
    void* acpi;
    void* memoryMap;
    size_t mapSize;
    size_t descriptorSize;
};
IDEDriver* ideDriver;
extern "C" void kernel_main(BootData data)
{
    asm volatile ("cli");
#ifdef __DEBUG__
    qemu_init();
    qemu_printf("Entered kernel.\n");
#endif
    initializeGDT();
#ifdef __DEBUG__
    qemu_printf("Initialized GDT.\n");
#endif
    initializeIDT();
    initializePIC();
#ifdef __DEBUG__
    qemu_printf("Initialized IDT & PIC.\n");
#endif
    initializePIT(50);
#ifdef __DEBUG__
    qemu_printf("Initialized PIT.\n");
#endif
    initializeMMU(data.memoryMap, data.mapSize, data.descriptorSize);
#ifdef __DEBUG__
    qemu_printf("Initialized MMU.\n");
#endif
    initializeHeap();
#ifdef __DEBUG__
    size_t* ls = new size_t[5];
    qemu_printf("Initialized Heap.\n");
    qemu_printf("Address of ls: 0x%x\n", ls);
#endif
    initializeScheduler();
#ifdef __DEBUG__
    qemu_printf("Initialized Scheduler.\n");
#endif
    asm volatile ("sti");
    initializePCI();
    drivers = Vector<Driver*>();
    ideDriver = NULL;
    for (size_t i = 0; i < pciDevices.size(); i++)
    {
        if (pciDevices[i].getClass() == 1 && pciDevices[i].getSubclass() == 1)
        {
            ideDriver = new IDEDriver(pciDevices[i]);
        }
    }
    if (ideDriver)
    {
#ifdef __DEBUG__
        uint8_t data[512];
        storageDevices[0]->readSectors(0, data, 1);
        for (size_t i = 0; i < 512; i++)
        {
            uint64_t byte = data[i];
            qemu_printf("0x%x ", byte);
        }
        qemu_printf("\n");
#endif
    }
    initializeGraphics(data.gop);
    Window* window = generateWindow(0, 0, 100, 100);
    for (;;);
}