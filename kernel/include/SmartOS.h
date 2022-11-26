#ifndef SMARTOS_H
#define SMARTOS_H
#include <efi.h>
#include <efilib.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C"
{
#endif
#ifdef __DEBUG__
void qemu_init();
void qemu_printf(const char* frmt, ...);
#endif
inline uint8_t inb(uint16_t port)
{
    uint8_t data;
    asm volatile("in %%dx, %%al": "=a"(data): "d"(port));
    return data;
}
inline void outb(uint16_t port, uint8_t data)
{
    asm volatile("out %%al, %%dx": : "a"(data), "d"(port));
}
char* itoa(int num, size_t base);
char* uitoa(size_t num, size_t base);
#ifdef __cplusplus
}
struct __attribute__((packed)) SystemPointer
{
    uint16_t limit;
    uint64_t base;
};
#endif
#endif