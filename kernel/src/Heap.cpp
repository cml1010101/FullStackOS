#include <Heap.h>
#include <MMU.h>
size_t totalAllocated;
Heap* currentHeap;
Heap::Heap(size_t size)
{
    this->size = size;
    first = (HeapEntry*)kmalloc_a(size);
    first->magic = 0xCAFEBEEFBEEFCAFE;
    first->free = true;
    first->size = size - sizeof(HeapEntry);
    first->next = NULL;
    totalAllocated = 0;
}
void Heap::split(HeapEntry* entry, size_t size)
{
    HeapEntry* next = (HeapEntry*)((char*)entry + size + sizeof(HeapEntry));
    next->free = true;
    next->next = entry->next;
    next->size = size - (entry->size + sizeof(HeapEntry));
    next->magic = 0xCAFEBEEFBEEFCAFE;
    entry->next = next;
    entry->size = size;
    entry->free = false;
}
void Heap::clean()
{
    HeapEntry* entry = first;
    while (entry->next)
    {
        if (entry->magic != 0xCAFEBEEFBEEFCAFE)
        {
            qemu_printf("Heap overrun\n");
            return;
        }
        HeapEntry* next = entry->next;
        if (entry->free && next->free)
        {
            entry->size += next->size + sizeof(HeapEntry);
            entry->next = next->next;
        }
        if (!entry->next) return;
        entry = next;
    }
}
void Heap::free(void* ptr)
{
    HeapEntry* entry = (HeapEntry*)ptr - 1;
    entry->free = 1;
    clean();
}
void* Heap::malloc(size_t size)
{
    if (size == 0) return NULL;
    totalAllocated += size;
    qemu_printf("Allocating %d bytes: %d total\n", size, totalAllocated);
    HeapEntry* entry = first;
    void* result;
    while ((entry->size < size || !entry->free) && entry->next)
    {
        if (entry->magic != 0xCAFEBEEFBEEFCAFE)
        {
            qemu_printf("Heap overrun\n");
            return NULL;
        }
        entry = entry->next;
    }
    if (entry->size >= size && entry->size <= (size + sizeof(HeapEntry)))
    {
        result = &entry[1];
        entry->free = 0;
        return result;
    }
    else if (entry->size > (size + sizeof(HeapEntry)))
    {
        split(entry, size);
        result = &entry[1];
        return result;
    }
    else
    {
        return NULL;
    }
}
void setCurrentHeap(Heap* heap)
{
    currentHeap = heap;
}
void* operator new(size_t size)
{
    return currentHeap->malloc(size);
}
void* operator new[](size_t size)
{
    return currentHeap->malloc(size);
}
void operator delete(void* ptr)
{
    currentHeap->free(ptr);
}
void operator delete[](void* ptr)
{
    currentHeap->free(ptr);
}
extern "C" void* malloc(size_t size)
{
    return currentHeap->malloc(size);
}
extern "C" void free(void* ptr)
{
    currentHeap->free(ptr);
}
Heap* kernelHeap;
void initializeHeap()
{
    kernelHeap = (Heap*)kmalloc(sizeof(Heap));
    *kernelHeap = Heap(0x1000000);
    setCurrentHeap(kernelHeap);
}