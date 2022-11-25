#include <SmartOS.h>
extern "C" void outb(uint16_t port, uint8_t data)
{
    asm volatile("out %%al, %%dx": : "a"(data), "d"(port));
}
extern "C" uint8_t inb(uint16_t port)
{
    uint8_t data;
    asm volatile("in %%dx, %%al": "=a"(data): "d"(port));
    return data;
}
extern "C" void swap(char* a, char* b)
{
    char c = *a;
    *a = *b;
    *b = c;
}
extern "C" void reverse(char* str, size_t length)
{
    int start = 0;
    int end = length - 1;
    while (start < end)
    {
        swap(str+start, str + end);
        start++;
        end--;
    }
}
char itoa_buffer[128];
char uitoa_buffer[128];
extern "C" size_t strlen(const char* str)
{
    size_t len;
    while (str[len])
    {
        len++;
    }
    return len;
}
extern "C" const char* uitoa(size_t num, size_t base)
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
extern "C" const char* itoa(int num, size_t base)
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
extern "C" void __cxa_pure_virtual()
{
}
extern "C" void memset(void* dest, uint8_t val, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        ((uint8_t*)dest)[i] = val;
    }
}
extern "C" void memcpy(void* dest, const void* src, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        ((uint8_t*)dest)[i] = ((uint8_t*)src)[i];
    }
}
extern "C" int memcmp(const void* a, const void* b, size_t count)
{
    int flag;
    for (size_t i = 0; i < count; i++)
    {
        if (flag = (((char*)a)[i] - ((char*)b)[i])) return flag;
    }
    return flag;
}