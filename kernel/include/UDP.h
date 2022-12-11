#ifndef UDP_H
#define UDP_H
#include <SmartOS.h>
#ifdef __cplusplus
struct UDPPacket
{
    uint16_t srcPort;
    uint16_t destPort;
    uint16_t length;
    uint16_t checksum;
    uint8_t data[];
};
void udpSendPacket(uint8_t* destIP, uint16_t srcPort, uint16_t destPort, void* data, size_t len,
    EthernetDevice* dev);
void udpHandlePacket(UDPPacket* packet, EthernetDevice* dev);
#endif
#endif