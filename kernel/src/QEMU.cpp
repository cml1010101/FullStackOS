#include <QEMU.h>
QEMUDriver qemu;
QEMUDriver::QEMUDriver()
{
    outb(QEMU_PORT + 1, 0x00);
    outb(QEMU_PORT + 3, 0x80);
    outb(QEMU_PORT + 0, 0x03);
    outb(QEMU_PORT + 1, 0x00);
    outb(QEMU_PORT + 3, 0x03);
    outb(QEMU_PORT + 2, 0xC7);
    outb(QEMU_PORT + 4, 0x0B);
}
bool QEMUDriver::isTransmitEmpty()
{
    return inb(QEMU_PORT + 5) & 0x20;
}
void QEMUDriver::putc(char c)
{
    while (!isTransmitEmpty());
    outb(QEMU_PORT, c);
}
void QEMUDriver::print(const char* str)
{
    size_t i = 0;
    while (str[i])
    {
        putc(str[i]);
        i++;
    }
}
void QEMUDriver::printf(const char* frmt, ...)
{
    va_list ls;
    va_start(ls, frmt);
    while (*frmt)
    {
        if (*frmt == '%')
        {
            frmt++;
            if (*frmt == '%')
            {
                putc('%');
            }
            else if (*frmt == 'd')
            {
                print(itoa(va_arg(ls, int), 10));
            }
            else if (*frmt == 'x')
            {
                print(uitoa(va_arg(ls, size_t), 16));
            }
            else if (*frmt == 's')
            {
                print(va_arg(ls, const char*));
            }
        }
        else putc(*frmt);
        frmt++;
    }
}
extern "C" void qemu_printf(const char* frmt, ...)
{
    va_list ls;
    va_start(ls, frmt);
    while (*frmt)
    {
        if (*frmt == '%')
        {
            frmt++;
            if (*frmt == '%')
            {
                qemu.putc('%');
            }
            else if (*frmt == 'd')
            {
                qemu.print(itoa(va_arg(ls, int), 10));
            }
            else if (*frmt == 'x')
            {
                qemu.print(uitoa(va_arg(ls, size_t), 16));
            }
            else if (*frmt == 's')
            {
                qemu.print(va_arg(ls, const char*));
            }
        }
        else qemu.putc(*frmt);
        frmt++;
    }
}
const char* QEMUDriver::getName()
{
    return "QEMU";
}