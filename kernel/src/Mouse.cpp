#include <Mouse.h>
#include <IRQ.h>
int mouseX, mouseY, mouseCycle;
uint8_t byte0;
void mouseWaitSignal()
{
    size_t timeout = 100000;
    while (timeout-- && !(inb(0x64) & 2));
}
void mouseWaitData()
{
    size_t timeout = 100000;
    while (timeout-- && !(inb(0x64) & 1));
}
void mouseWrite(uint8_t data)
{
    mouseWaitSignal();
    outb(0x64, 0xD4);
    mouseWaitSignal();
    outb(0x60, data);
}
uint8_t mouseRead()
{
    mouseWaitData();
    return inb(0x60);
}
void mouseInterrupt(CPURegisters* regs)
{
    switch (mouseCycle)
    {
    case 0:
        byte0 = inb(0x60);
        mouseCycle++;
        break;
    case 1:
        int8_t deltaX;
        asm volatile ("in %%dx, %%al": "=a"(deltaX): "d"(0x60));
        mouseX += deltaX;
        mouseX = min(max(mouseX, 0), 1920);
        mouseCycle++;
        break;
    case 2:
        int8_t deltaY;
        asm volatile ("in %%dx, %%al": "=a"(deltaY): "d"(0x60));
        mouseY -= deltaY;
        mouseY = min(max(mouseY, 0), 1080);
        mouseCycle = 0;
        break;
    }
}
void initializeMouse()
{
    mouseX = 50;
    mouseY = 50;
    registerIRQHandler(12, mouseInterrupt);
    mouseWaitSignal();
    outb(0x64, 0xA8);
    mouseWaitSignal();
    outb(0x64, 0x20);
    mouseWaitData();
    uint8_t status = inb(0x60) | 2;
    mouseWaitSignal();
    outb(0x64, 0x60);
    mouseWaitSignal();
    outb(0x60, status);
    mouseWrite(0xF6);
    mouseRead();
    mouseWrite(0xF4);
    mouseRead();
    mouseCycle = 0;
}