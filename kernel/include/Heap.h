#ifndef HEAP_H
#define HEAP_H
#include <SmartOS.h>
#ifdef __cplusplus
struct HeapEntry
{
    uint64_t magic; // 0xCAFEBEEFBEEFCAFE
    bool free;
    size_t size;
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
    void free(void* ptr);
};
void setCurrentHeap(Heap* heap);
void initializeHeap();
#endif
#endif