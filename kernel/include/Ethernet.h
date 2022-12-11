#ifndef ETHERNET_H
#define ETHERNET_H
#include <SmartOS.h>
#ifdef __cplusplus
#define ETHERNET_TYPE_ARP 0x0806
#define ETHERNET_TYPE_IP4 0x0800
#define HARDWARE_TYPE_ETHERNET 0x01
struct __attribute__((packed)) EthernetFrame
{
    uint8_t destMac[6];
    uint8_t srcMac[6];
    uint16_t type;
    uint8_t data[];
};
void initializeEthernet();
void sendEthernetPacket(const uint8_t* dest, uint8_t* data, size_t len, uint16_t protocol,
    EthernetDevice* dev);
void recieveEthernetPacket(EthernetFrame* frame, size_t len, EthernetDevice* dev);
void setSourceIP(uint8_t* ip);
uint8_t* getSourceIP();
#endif
#endif