#ifndef INTELCPU_H
#define INTELCPU_H
#include <SmartOS.h>
#ifdef __cplusplus
#define MMU_PRESENT 0x1
#define MMU_RW 0x2
#define MMU_USER 0x4
#define MMU_NO_CACHE 0x10
#define MMU_NO_EXEC (1 << 63)
#define MMU_ADDR 0x000FFFFFFFFFF000
struct PageDirectory
{
    uint64_t pml4[512];
    uint64_t phys;
    PageDirectory();
    void mapPage(size_t phys, size_t virt, uint64_t flags);
    void map(size_t phys, size_t virt, size_t pages, uint64_t flags);
};
class MMU
{
private:
    size_t kmallocPhys, kmallocStart, kmallocEnd;
    void* memoryMap;
    size_t memoryMapSize, descriptorSize;
    struct MemoryBlock
    {
        bool present;
        uint32_t type;
        size_t physStart;
        size_t numPages;
        uint8_t* bitmap;
    };
public:
    MemoryBlock blocks[64];
    PageDirectory* currentDirectory;
    PageDirectory* kernelDirectory;
    MMU() = default;
    MMU(void* memoryMap, size_t memoryMapSize, size_t descriptorSize);
    uint64_t kmalloc(size_t size, bool align = true);
    uint8_t getFrame(size_t i);
    void setFrame(size_t i);
    void clearFrame(size_t i);
    void setFrames(size_t i, size_t count);
    size_t allocFrame();
    size_t allocFrames(size_t size);
    void freeFrames(size_t i, size_t size);
    void summarize();
    void setDirectory(PageDirectory* dir);
};
struct CPURegisters
{
    size_t gs, es, fs, ds;
    size_t r15, r14, r13, r12, r11, r10, r9, r8, rbp, rdi, rsi, rdx, rcx, rbx, rax;
    size_t num, code;
    size_t rip, cs, rflags, rsp, ss;
};
struct __attribute__((packed)) SystemPointer
{
    uint16_t limit;
    uint64_t base;
};
struct __attribute__((packed)) IDTEntry
{
    uint16_t offset0;
    uint16_t selector;
    uint8_t ist: 3;
    uint8_t rsv0: 5;
    uint8_t type: 5;
    uint8_t dpl: 2;
    uint8_t p: 1;
    uint16_t offset1;
    uint32_t offset2;
    uint32_t rsv1;
    IDTEntry();
    IDTEntry(uint64_t offset, uint16_t selector, uint8_t type, uint8_t dpl, uint8_t ist);
};
typedef void(*IRQHandler)(CPURegisters*);
class IDT
{
private:
    IDTEntry entries[256];
    SystemPointer address;
public:
    IDT();
    IDTEntry operator[](size_t i) const;
    IDTEntry& operator[](size_t i);
    void load();
};
struct GDTEntry
{
    uint16_t limit0;
    uint16_t base0;
    uint8_t base1;
    uint8_t access;
    uint8_t limit1: 4;
    uint8_t flags: 4;
    uint8_t base2;
    GDTEntry();
    GDTEntry(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
};
class GDT
{
private:
    GDTEntry entries[20];
    SystemPointer address;
public:
    GDT();
    void load(size_t cs, size_t ds, size_t es, size_t fs, size_t gs, size_t ss);
    GDTEntry operator[](size_t i) const;
    GDTEntry& operator[](size_t i);
};
#define LAPIC_ID 0x8
#define LAPIC_VERSION 0xC
#define APIC_TPR 0x20
#define APIC_APR 0x24
#define APIC_PPR 0x28
#define APIC_EOI 0x2C
#define APIC_RRD 0x30
#define APIC_LDR 0x34
#define APIC_DFR 0x38
#define APIC_SIVR 0x3C
#define APIC_ISR 0x40
#define APIC_TMR 0x60
#define APIC_IRR 0x80
#define APIC_ESR 0xA0
#define APIC_CMCI 0xBC
#define APIC_ICR 0xC0
#define APIC_TIMER 0xC8
#define APIC_THERMAL 0xCC
#define APIC_PMCR 0xD0
#define APIC_LINT0 0xD4
#define APIC_LINT1 0xD8
#define APIC_LERR 0xDC
#define APIC_ICOUNT 0xE0
#define APIC_CCOUNT 0xE4
#define APIC_DCOUNT 0xF8
class CPU : public Driver
{
private:
    bool initialized;
    uint32_t featuresECX, featuresEDX;
public:
    uint32_t* apic;
    GDT gdt;
    IDT idt;
    MMU mmu;
    IRQHandler irqHandlers[16];
    CPU() = default;
    CPU(void* memoryMap, size_t memoryMapSize, size_t descriptorSize);
    bool supportsCPUID();
    const char* getVendor();
    const char* getName();
    void loadFeatures();
    void setupGDT();
    void setupIDT();
    void setupPaging();
    bool supportAPIC();
    void readMSR(uint32_t reg, uint32_t& eax, uint32_t& edx);
    void writeMSR(uint32_t reg, uint32_t eax, uint32_t edx);
    void setAPICBase(uint64_t base);
    uint64_t getAPICBase();
    void enableAPIC();
};
extern CPU cpu;
#endif
#endif