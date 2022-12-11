#ifndef ETHERNET_H
#define ETHERNET_H
#include <SmartOS.h>
#ifdef __cplusplus
#define ETHERNET_TYPE_ARP 0x0806
#define ETHERNET_TYPE_IP4 0x0800
#define ETHERNET_TYPE_IP6 0x86DD
#define HARDWARE_TYPE_ETHERNET 0x01
struct __attribute__((packed)) EthernetFrame
{
    uint8_t destMac[6];
    uint8_t srcMac[6];
    uint16_t type;
    uint8_t data[];
};
typedef void(*ProtocolHandlerFunc)(const void* data, size_t dataLength, EthernetDevice* dev);
struct ProtocolHandler
{
    ProtocolHandlerFunc func;
    uint16_t type;
    inline ProtocolHandler(uint16_t type, ProtocolHandlerFunc func)
    {
        this->func = func;
        this->type = type;
    }
};
typedef void(*ProtocolInit)(EthernetDevice* dev);
void initializeEthernet();
void addEthernetProtocol(ProtocolInit initFunc, ProtocolHandler handlerFunc);
void addNIC(PCIDevice* dev);
void sendEthernetPacket(const uint8_t* dest, uint8_t* data, size_t len, uint16_t protocol,
    EthernetDevice* dev);
void recieveEthernetPacket(EthernetFrame* frame, size_t len, EthernetDevice* dev);
void setSourceIP4(uint8_t* ip);
uint8_t* getSourceIP4();
void setSourceIP6(uint8_t* ip);
uint8_t* getSourceIP6();
#endif
#endif