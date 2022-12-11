#include <IRQ.h>
IRQHandler handlers[16];
bool apicEnabled;
void initializePIC()
{
    apicEnabled = false;
    for (size_t i = 0; i < 16; i++)
    {
        handlers[i] = 0;
    }
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}
void initializeAPIC()
{
    apicEnabled = true;
}
void registerIRQHandler(uint32_t irqNum, IRQHandler handler)
{
    handlers[irqNum] = handler;
}
extern "C" void irq_handler(CPURegisters* regs)
{
    if (!apicEnabled)
    {
        if (regs->num >= 8) outb(0xA0, 0x20);
        outb(0x20, 0x20);
    }
    if (regs->num != 0) qemu_printf("IRQ%d\n", regs->num);
    if (handlers[regs->num]) handlers[regs->num](regs);
    if (apicEnabled)
    {
    }
}