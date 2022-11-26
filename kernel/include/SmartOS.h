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
    asm volatile ("in %%dx, %%al": "=a"(data): "d"(port));
    return data;
}
inline void outb(uint16_t port, uint8_t data)
{
    asm volatile ("out %%al, %%dx":: "a"(data), "d"(port));
}
inline uint32_t inl(uint16_t port)
{
    uint32_t data;
    asm volatile ("in %%dx, %%eax": "=a"(data): "d"(port));
    return data;
}
inline void outl(uint16_t port, uint32_t data)
{
    asm volatile ("out %%eax, %%dx":: "a"(data), "d"(port));
}
char* itoa(int num, size_t base);
char* uitoa(size_t num, size_t base);
void memset(void* dest, uint8_t val, size_t count);
void memcpy(void* dest, const void* src, size_t count);
int memcmp(const void* a, const void* b, size_t count);
void* malloc(size_t size);
void free(void* ptr);
void sleep(uint64_t millis);
#ifdef __cplusplus
}
struct __attribute__((packed)) SystemPointer
{
    uint16_t limit;
    uint64_t base;
};
struct CPURegisters
{
    size_t gs, es, fs, ds;
    size_t r15, r14, r13, r12, r11, r10, r9, r8, rbp, rdi, rsi, rdx, rcx, rbx, rax;
    size_t num, code;
    size_t rip, cs, rflags, rsp, ss;
};
template<typename T>
class Vector
{
private:
    struct VectorItem
    {
        T t;
        VectorItem* next;
    };
    VectorItem* first;
    size_t length;
public:
    inline Vector()
    {
        first = NULL;
        length = 0;
    }
    inline T operator[](size_t index) const
    {
        VectorItem* item = first;
        for (size_t i = 0; i < index; i++)
        {
            item = item->next;
        }
        return item->t;
    }
    inline T& operator[](size_t index)
    {
        VectorItem* item = first;
        for (size_t i = 0; i < index; i++)
        {
            item = item->next;
        }
        return item->t;
    }
    inline void push(T t)
    {
        if (length != 0)
        {
            VectorItem* item = first;
            while (item->next) item = item->next;
            item->next = new VectorItem;
            item->next->next = NULL;
            item->next->t = t;
        }
        else
        {
            first = new VectorItem;
            first->next = NULL;
            first->t = t;
        }
        length++;
    }
    inline T pop()
    {
        T t;
        if (length != 1)
        {
            VectorItem* item = first;
            while (item->next->next) item = item->next;
            t = item->next->t;
            item->next = NULL;
        }
        else
        {
            t = first->t;
            first = NULL;
        }
        length--;
        return t;
    }
    inline size_t size()
    {
        return length;
    }
};
class Driver
{
public:
    Driver() = default;
    virtual const char* getName() = 0;
};
void registerDriver(Driver* driver);
extern Vector<Driver*> drivers;
class StorageDevice
{
public:
    StorageDevice() = default;
    virtual const char* getName() = 0;
    virtual const char* getType() = 0;
    virtual void readSectors(uint64_t lba, void* dest, size_t sectors) = 0;
    virtual void writeSectors(uint64_t lba, const void* src, size_t sectors) = 0;
};
extern Vector<StorageDevice*> storageDevices;
void registerStorageDevice(StorageDevice* device);
#endif
#endif