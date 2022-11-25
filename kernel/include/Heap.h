#ifndef HEAP_H
#define HEAP_H
#include <SmartOS.h>
#ifdef __cplusplus
struct HeapEntry
{
    size_t size;
    HeapEntry* next;
    bool free;
};
class Heap
{
private:
    HeapEntry* heapStart;
    size_t heapSize;
    void splitHeap(HeapEntry* entry, size_t size);
    void clean();
public:
    Heap();
    void* malloc(size_t size);
    void free(void* ptr);
};
void setCurrentHeap(Heap* heap);
extern Heap* heap;
#endif
#endif