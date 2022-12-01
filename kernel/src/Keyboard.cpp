#include <Keyboard.h>
#include <IRQ.h>
Vector<KeyboardHandler> keyHandlers;
KeyboardState keyboardState;
void keyWaitSignal()
{
    size_t timeout = 100000;
    while (timeout-- && !(inb(0x64) & 2));
}
void keyWaitData()
{
    size_t timeout = 100000;
    while (timeout-- && !(inb(0x64) & 1));
}
uint8_t keyRead()
{
    keyWaitSignal();
    return inb(0x60);
}
void keyWrite(uint8_t cmd, uint8_t data)
{
    keyWaitSignal();
    outb(0x64, 0xD2);
    keyWaitSignal();
    outb(0x60, cmd);
    keyRead();
    keyWaitSignal();
    outb(0x64, 0xD2);
    keyWaitSignal();
    outb(0x60, data);
    keyRead();
}
void keyboardIRQ(CPURegisters* regs)
{
    uint8_t pressed = 1;
    uint8_t key = inb(0x60);
    if (key & 0x80)
    {
        pressed = 0;
        key &= 0x7F;
    }
    if (key == KEY_SHIFT)
    {
        keyboardState.leftShift = pressed;
    }
    for (size_t i = 0; i < keyHandlers.size(); i++)
    {
        keyHandlers[i](key, pressed);
    }
}
void initializeKeyboard()
{
    registerIRQHandler(1, keyboardIRQ);
    keyWaitSignal();
    outb(0x64, 0xAE);
    keyWaitSignal();
    outb(0x64, 0x20);
    keyWaitData();
    uint8_t status = inb(0x60) | 1;
    keyWaitSignal();
    outb(0x64, 0x60);
    keyWaitSignal();
    outb(0x60, status);
    keyWrite(0xF6, 0);
    uint8_t dat = keyRead();
    qemu_printf("%d\n", dat);
    memset(&keyboardState, 0, sizeof(KeyboardState));
    keyHandlers = Vector<KeyboardHandler>();
}
void addKeyboardHandler(KeyboardHandler handler)
{
    keyHandlers.push(handler);
}