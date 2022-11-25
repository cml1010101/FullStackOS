#include <SmartOS.h>
#ifdef DEBUG
extern "C" void qemu_putc(char c)
{
    while (inb(0x3F8 + 5) & 0x20);
    outb(0x3F8)
}
#endif