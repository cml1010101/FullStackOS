#ifndef RTL8169_H
#define RTL8169_H
#include <SmartOS.h>
#include <PCI.h>
#ifdef __cplusplus
#define RTL8169_CMD 0x37
#define DESCRIPTOR_EOR 0x40000000
#define DESCRIPTOR_OWN 0x80000000
struct __attribute__((packed)) Descriptor
{
    uint32_t command;
    uint32_t vlan;
    uint32_t lowAddr;
    uint32_t highAddr;
};
class RTL8169
{
private:
    uint16_t ioBase;
    Descriptor* rxDescs;
    Descriptor* txDescs;
public:
    RTL8169(PCIDevice* dev);
};
#endif
#endif