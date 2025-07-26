#include <Heap.h>
#include <MMU.h>
uint64_t calculateChecksum(HeapEntry* entry)
{
    size_t* ptr = &((size_t*)entry)[1];
    size_t sum = ptr[0] + ptr[1];
    return ~sum;
}
Heap* currentHeap;
Heap::Heap(size_t size)
{
    this->size = size;
    first = (HeapEntry*)kmalloc_a(size);
    first->free = 1;
    first->size = size - sizeof(HeapEntry);
    first->next = NULL;
    first->checksum = calculateChecksum(first);
}
void Heap::split(HeapEntry* entry, size_t size)
{
    HeapEntry* next = (HeapEntry*)((char*)entry + size + sizeof(HeapEntry));
    next->free = 1;
    next->next = entry->next;
    next->size = entry->size - (size + sizeof(HeapEntry));
    next->checksum = calculateChecksum(next);
    entry->next = next;
    entry->size = size;
    entry->free = 0;
    entry->checksum = calculateChecksum(entry);
}
void Heap::summary()
{
    qemu_printf("Summary of HEAP:\n");
    HeapEntry* entry = first;
    while (entry)
    {
        if (entry->checksum != calculateChecksum(entry))
        {
            qemu_printf("%d bytes at 0x%x broken. [%s]\n", entry->size, &entry[1], entry->free ? 
                "FREE" : "UNAVAILABLE");
            break;
        }
        qemu_printf("%d bytes at 0x%x. [%s]\n", entry->size, &entry[1], entry->free ? 
            "FREE" : "UNAVAILABLE");
        entry = entry->next;
    }
}
void Heap::clean()
{
    HeapEntry* entry = first;
    while (entry->next)
    {
        if (entry->checksum != calculateChecksum(entry))
        {
            qemu_printf("Heap checksum invalid during 'clean'. QUIT.\n");
            summary();
            asm volatile ("cli; jmp .");
        }
        HeapEntry* next = entry->next;
        if (entry->free && next->free)
        {
            entry->size += next->size + sizeof(HeapEntry);
            entry->next = next->next;
            entry->checksum = calculateChecksum(entry);
        }
        entry = entry->next;
    }
}
void Heap::free(void* ptr)
{
    HeapEntry* entry = ((HeapEntry*)ptr) - 1;
    entry->free = 1;
    entry->checksum = calculateChecksum(entry);
    clean();
}
void* Heap::malloc(size_t size)
{
    if (size == 0) return NULL;
    HeapEntry* entry = first;
    void* result;
    while ((entry->size < size || !entry->free) && entry->next)
    {
        if (entry->checksum != calculateChecksum(entry))
        {
            qemu_printf("Heap checksum invalid during 'malloc(%d)'. QUIT.\n", size);
            summary();
            asm volatile ("cli; jmp .");
        }
        entry = entry->next;
    }
    if (entry->checksum != calculateChecksum(entry))
    {
        qemu_printf("Heap checksum invalid during 'malloc(%d)'. QUIT.\n", size);
        summary();
        asm volatile ("cli; jmp .");
    }
    if (entry->size >= size && entry->size <= (size + sizeof(HeapEntry)))
    {
        result = &entry[1];
        entry->free = 0;
        entry->checksum = calculateChecksum(entry);
        clean();
        return result;
    }
    else if (entry->size > (size + sizeof(HeapEntry)))
    {
        split(entry, size);
        result = &entry[1];
        clean();
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
void operator delete(void* ptr, size_t size)
{
    (void)size; // Unused parameter
    currentHeap->free(ptr);
}
void operator delete[](void* ptr, size_t size)
{
    (void)size; // Unused parameter
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