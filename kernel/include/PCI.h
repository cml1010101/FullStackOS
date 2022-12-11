#ifndef PCI_H
#define PCI_H
#include <SmartOS.h>
#ifdef __cplusplus
class PCIDevice
{
private:
    uint8_t bus, slot, func;
public:
    PCIDevice() = default;
    PCIDevice(uint8_t bus, uint8_t slot, uint8_t func);
    uint16_t getVendor();
    uint16_t getDeviceID();
    uint8_t getClass();
    uint8_t getSubclass();
    uint8_t getProgIF();
    uint8_t getHeaderType();
    uint32_t getBar(size_t bar);
    uint8_t getInterrupt();
    uint16_t getCommand();
    void writeCommand(uint16_t cmd);
};
extern Vector<PCIDevice> pciDevices;
void initializePCI();
#endif
#endif