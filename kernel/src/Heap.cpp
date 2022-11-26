#include <Heap.h>
#include <MMU.h>
Heap* currentHeap;
Heap::Heap(size_t size)
{
    this->size = size;
    first = (HeapEntry*)kmalloc(size);
    first->free = true;
    first->size = size - sizeof(HeapEntry);
    first->next = NULL;
}
void Heap::split(HeapEntry* entry, size_t size)
{
    HeapEntry* next = (HeapEntry*)((char*)entry + size + sizeof(HeapEntry));
    next->free = true;
    next->next = entry->next;
    next->size = size - (entry->size + sizeof(HeapEntry));
    entry->next = next;
    entry->size = size;
    entry->free = false;
}
void Heap::clean()
{
    HeapEntry* entry = first;
    while (entry->next)
    {
        HeapEntry* next = (HeapEntry*)((uint8_t*)entry + sizeof(HeapEntry) + entry->size);
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
    if (size == 0)
    {
        return NULL;
    }
    HeapEntry* entry = first;
    void* result;
    while ((entry->size < size || !entry->free) && entry->next)
    {
        entry = (HeapEntry*)((uint8_t*)entry + sizeof(HeapEntry) + entry->size);
    }
    if (entry->size >= size && entry->size <= (size + sizeof(HeapEntry)))
    {
        result = entry + 1;
        entry->free = 0;
        return result;
    }
    else if (entry->size > (size + sizeof(HeapEntry)))
    {
        split(entry, size);
        result = entry + 1;
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
Heap kernelHeap;
void initializeHeap()
{
    kernelHeap = Heap(0x10000);
    setCurrentHeap(&kernelHeap);
}