#ifndef UDP_H
#define UDP_H
#include <SmartOS.h>
#ifdef __cplusplus
<<<<<<< HEAD
struct __attribute__((packed)) UDPPacket
{
    uint16_t src_port;
    uint16_t dst_port;
=======
struct UDPPacket
{
    uint16_t srcPort;
    uint16_t destPort;
>>>>>>> parent of 2c35866... fixes
    uint16_t length;
    uint16_t checksum;
    uint8_t data[];
};
<<<<<<< HEAD
void udpHandle(const void* data, size_t length, EthernetDevice* dev);
void udpSend(InternetAddr* addr, uint16_t srcPort, uint16_t destPort, const void* data, size_t len,
    EthernetDevice* dev);
=======
void udpSendPacket(uint8_t* destIP, uint16_t srcPort, uint16_t destPort, void* data, size_t len,
    EthernetDevice* dev);
void udpHandlePacket(UDPPacket* packet, EthernetDevice* dev);
>>>>>>> parent of 2c35866... fixes
#endif
#endif