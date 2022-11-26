#include <SmartOS.h>
#ifdef __DEBUG__
#define QEMU_PORT 0x3F8
extern "C" void qemu_init()
{
    outb(QEMU_PORT + 1, 0x00);
    outb(QEMU_PORT + 3, 0x80);
    outb(QEMU_PORT + 0, 0x03);
    outb(QEMU_PORT + 1, 0x00);
    outb(QEMU_PORT + 3, 0x03);
    outb(QEMU_PORT + 2, 0xC7);
    outb(QEMU_PORT + 4, 0x0B);
}
extern "C" void qemu_putc(char c)
{
    while ((inb(QEMU_PORT + 5) & 0x20) == 0);
    outb(QEMU_PORT, c);
}
extern "C" void qemu_print(const char* str)
{
    size_t i = 0;
    while (str[i])
    {
        qemu_putc(str[i]);
        i++;
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
            if (*frmt == 'd')
            {
                qemu_print(itoa(va_arg(ls, int), 10));
            }
            else if (*frmt == 'u')
            {
                qemu_print(uitoa(va_arg(ls, uint32_t), 10));
            }
            else if (*frmt == 'x')
            {
                qemu_print(uitoa(va_arg(ls, size_t), 16));
            }
            else if (*frmt == '%')
            {
                qemu_putc('%');
            }
            else if (*frmt == 's')
            {
                qemu_print(va_arg(ls, const char*));
            }
            else if (*frmt == 'c')
            {
                qemu_putc(va_arg(ls, char));
            }
        }
        else
        {
            qemu_putc(*frmt);
        }
        frmt++;
    }
}
#endif
char itoa_buffer[50];
char uitoa_buffer[50];
extern "C" void swap(char* a, char* b)
{
    char c = *a;
    *a = *b;
    *b = c;
}
extern "C" void reverse(char* str, int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
        swap(str+start, str + end);
        start++;
        end--;
    }
}
extern "C" char* uitoa(size_t num, size_t base)
{
    char* str;
    int i = 0;
    if (num == 0)
    {
        str = uitoa_buffer;
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
    size_t len = 0;
    size_t n = num;
    while (n != 0)
    {
        len++;
        n = n / base;
    }
    str = uitoa_buffer;
    while (num != 0)
    {
        size_t rem = num % base;
        str[i++] = (rem > 9) ? (rem-10) + 'a' : rem + '0';
        num = num / base;
    }
    str[i] = '\0';
    reverse(str, i);
    return str;
}
char* itoa(int num, size_t base)
{
    char* str;
    int i = 0;
    int isNegative = 0;
    if (num == 0)
    {
        str = itoa_buffer;
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
    size_t len = 0;
    if (num < 0 && base == 10)
    {
        len = 1;
        isNegative = 1;
        num = -num;
    }
    int n = num;
    while (n != 0)
    {
        len++;
        n = n / base;
    }
    str = itoa_buffer;
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem-10) + 'a' : rem + '0';
        num = num / base;
    }
    if (isNegative)
        str[i++] = '-';
    str[i] = '\0';
    reverse(str, i);
    return str;
}
extern "C" void memset(void* dest, uint8_t val, size_t size)
{
    for (size_t i = 0; i < size; i++) ((char*)dest)[i] = val;
}
extern "C" void memcpy(void* dest, const void* src, size_t len)
{
    for (size_t i = 0; i < len; i++) ((char*)dest)[i] = ((const char*)src)[i];
}
extern "C" int memcmp(const void* a, const void* b, size_t len)
{
    const char* a_str = (const char*)a;
    const char* b_str = (const char*)b;
    for (size_t i = 0; i < len; i++)
    {
        if (a_str[i] - b_str[i]) return a_str[i] - b_str[i];
    }
    return 0;
}
extern "C" void __cxa_pure_virtual()
{
}
Vector<Driver*> drivers;
void registerDriver(Driver* driver)
{
    drivers.push(driver);
}
Vector<StorageDevice*> storageDevices;
void registerStorageDevice(StorageDevice* device)
{
    storageDevices.push(device);
}