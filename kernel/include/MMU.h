#ifndef MMU_H
#define MMU_H
#include <SmartOS.h>
#define MMU_PRESENT 0x1
#define MMU_RW 0x2
#define MMU_USER 0x4
#define MMU_ADDR 0x000FFFFFFFFFF000
#ifdef __cplusplus
class PageDirectory
{
public:
    uint64_t pml4[512];
    PageDirectory();
    void mapPage(uint64_t virt, uint64_t phys, uint64_t flags);
    void map(uint64_t virt, uint64_t phys, size_t pages, size_t flags);
};
void initializeMMU(void* memoryMap, size_t memoryMapSize, size_t descriptorSize);
#endif
#endif