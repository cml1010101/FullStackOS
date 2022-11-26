#include <PIT.h>
#include <IRQ.h>
size_t counter;
void pitHandler(CPURegisters* regs)
{
    counter++;
}
void initializePIT(size_t frequency)
{
    uint64_t divisor = 1193180 / frequency;
    outb(0x43, 0x36);
    uint8_t l = divisor & 0xFF;
    uint8_t h = (divisor >> 8) & 0xFF;
    outb(0x40, l);
    outb(0x40, h);
    counter = 0;
    registerIRQHandler(0, pitHandler);
}