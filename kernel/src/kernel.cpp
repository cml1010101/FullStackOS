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
#include <Partitions.h>
#include <Targa.h>
#include <Mouse.h>
#include <Keyboard.h>
struct BootData
{
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
    void* acpi;
    void* memoryMap;
    size_t mapSize;
    size_t descriptorSize;
};
IDEDriver* ideDriver;
#ifdef __DEBUG__
extern "C" void handleKey(uint8_t key, uint8_t pressed)
{
    if (pressed)
    {
        char c = keycodesASCII[key];
        if (keyboardState.leftShift) c = keycodesShiftASCII[key];
        if (c)
        {
            char str[2];
            str[0] = c;
            str[1] = 0;
            qemu_printf("%s", str);
        }
    }
}
#endif
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
    initializePIT(100);
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
    if (ideDriver) registerDriver(ideDriver);
    fileSystems = Vector<FileSystem*>();
    for (size_t i = 0; i < storageDevices.size(); i++)
    {
        if (storageDevices[i]->getSize())
        {
            scanDevice(storageDevices[i]);
        }
    }
    initializeGraphics(data.gop);
    Window* window = generateWindow(0, 0, 100, 100);
    File* font_psf = fileSystems[0]->open("RES/FONT.PSF");
    uint8_t* font_data = new uint8_t[font_psf->getSize()];
    font_psf->read(font_data, font_psf->getSize());
    Font font = Font(font_data);
    File* cursor_tga = fileSystems[0]->open("RES/CURSOR.TGA");
    uint8_t* cursor_data = new uint8_t[cursor_tga->getSize()];
    cursor_tga->read(cursor_data, cursor_tga->getSize());
    Icon* cursorMap = parseTGA(cursor_data, cursor_tga->getSize());
    setCursor(cursorMap);
    qemu_printf("Initializing mouse.\n");
    initializeMouse();
    qemu_printf("Initialized mouse.\n");
    initializeCursor();
    window->drawChar(20, 20, 'H', 0, &font);
    initializeKeyboard();
#ifdef __DEBUG__
    addKeyboardHandler(handleKey);
#endif
    for (;;);
}