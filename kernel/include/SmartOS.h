#ifndef SMARTOS_H
#define SMARTOS_H
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <efi.h>
#include <efilib.h>
#ifdef __cplusplus
extern "C"
{
#endif
void outb(uint16_t port, uint8_t data);
uint8_t inb(uint16_t port);
const char* itoa(int num, size_t base);
const char* uitoa(size_t num, size_t base);
void reverse(char* str, size_t len);
void memset(void* dest, uint8_t val, size_t count);
int memcmp(const void* a, const void* b, size_t count);
void* malloc(size_t size);
void free(void* ptr);
inline double abs(double x)
{
    return (x > 0) ? x : -x;
}
#ifdef __cplusplus
}
class Driver
{
public:
    Driver() = default;
    virtual const char* getName() = 0;
};
template<typename T>
class Vector
{
private:
    struct VectorItem
    {
        T t;
        struct Vector<T>::VectorItem* next;
    };
    Vector<T>::VectorItem* listStart;
    size_t size;
public:
    inline Vector()
    {
        size = 0;
        listStart = NULL;
    }
    inline T operator[](size_t i) const
    {
        VectorItem* item = listStart;
        for (size_t j = 0; j < i; j++)
        {
            item = item->next;
        }
        return item->t;
    }
    inline T& operator[](size_t i)
    {
        VectorItem* item = listStart;
        for (size_t j = 0; j < i; j++)
        {
            item = item->next;
        }
        return item->t;
    }
    inline void push(T t)
    {
        VectorItem* item = listStart;
        while (item->next)
        {
            item = item->next;
        }
        VectorItem* next = new VectorItem;
        next->next = NULL;
        next->t = t;
        item->next = next;
    }
    inline T pop()
    {
        VectorItem* item = listStart;
        while (item->next && item->next->next)
        {
            item = item->next;
        }
        T t = item->next->t;
        size--;
        delete item.next;
        item->next = item->next->next;
        return t;
    }
    inline size_t length()
    {
        return size;
    }
};
#endif
#endif