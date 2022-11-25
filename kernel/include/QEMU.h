#ifndef QEMU_H
#define QEMU_H
#include <SmartOS.h>
#define QEMU_PORT 0x3F8
#ifdef __cplusplus
extern "C"
{
#endif
void qemu_printf(const char* frmt, ...);
#ifdef __cplusplus
}
class QEMUDriver : public Driver
{
private:
    bool isTransmitEmpty();
public:
    QEMUDriver();
    void putc(char c);
    void print(const char* str);
    void printf(const char* frmt, ...);
    const char* getName();
};
extern QEMUDriver qemu;
struct BootData
{
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
    void* acpi;
    void* memoryMap;
    size_t mapSize;
    size_t descriptorSize;
};
#endif
#endif