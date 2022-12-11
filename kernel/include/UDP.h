#ifndef UDP_H
#define UDP_H
#include <SmartOS.h>
#ifdef __cplusplus
struct __attribute__((packed)) UDPPacket
{
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
    uint8_t data[];
};
void udpHandle(const void* data, size_t length, EthernetDevice* dev);
void udpSend(InternetAddr* addr, uint16_t srcPort, uint16_t destPort, const void* data, size_t len,
    EthernetDevice* dev);
#endif
#endif