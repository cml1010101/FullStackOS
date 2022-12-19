#ifndef HEAP_H
#define HEAP_H
#include <SmartOS.h>
#ifdef __cplusplus
struct HeapEntry
{
    size_t checksum;
    size_t free: 1;
    size_t size: 63;
    struct HeapEntry* next;
};
class Heap
{
private:
    HeapEntry* first;
    size_t size;
    void split(HeapEntry* entry, size_t size);
    void clean();
public:
    Heap() = default;
    Heap(size_t size);
    void* malloc(size_t size);
    void summary();
    void free(void* ptr);
};
void setCurrentHeap(Heap* heap);
void initializeHeap();
#endif
#endif