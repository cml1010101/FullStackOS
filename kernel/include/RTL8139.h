#ifndef RTL8139_H
#define RTL8139_H
#include <SmartOS.h>
#include <PCI.h>
#ifdef __cplusplus
#define RTL8139_CMD 0x37
#define DESCRIPTOR_EOR 0x40000000
#define DESCRIPTOR_OWN 0x80000000
#define NUM_RX 4
#define RX_BUFFER_SIZE 100
class RTL8139 : public EthernetDevice
{
public:
    uint16_t ioBase;
    void* rxDescs;
    uint8_t macAddress[6];
    uint64_t rxPtr, packetIdx;
    volatile uint8_t packageACK;
    RTL8139(PCIDevice* dev);
    void sendPacket(Package* networkPackage);
    Package* recievePacket();
    uint8_t* getMAC();
};
extern RTL8139* defaultRTL8139;
#endif
#endif