#include <Heap.h>
#include <CPU.h>
Heap::Heap()
{
    heapSize = 0x10 * 0x1000;
    heapStart = (HeapEntry*)cpu.mmu.kmalloc(heapSize);
    heapStart->size = heapSize - sizeof(HeapEntry);
    heapStart->next = NULL;
    heapStart->free = true;
}
void* Heap::malloc(size_t size)
{
    if (size == 0)
    {
        return NULL;
    }
    HeapEntry* entry = heapStart;
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
        splitHeap(entry, size);
        result = entry + 1;
        return result;
    }
    else
    {
        return NULL;
    }
}
void Heap::splitHeap(HeapEntry* entry, size_t size)
{
    HeapEntry* next = (HeapEntry*)((uint8_t*)entry + sizeof(HeapEntry) + size);
    next->free = true;
    next->next = entry->next;
    next->size = entry->size - size - sizeof(HeapEntry);
    entry->free = false;
    entry->size = size;
    entry->next = next;
}
void Heap::clean()
{
    HeapEntry* entry = heapStart;
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
Heap* currentHeap;
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