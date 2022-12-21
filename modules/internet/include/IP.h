#ifndef IP_H
#define IP_H
#include <SmartOS.h>
#ifdef __cplusplus
#define PROTOCOL_UDP 17
#define PROTOCOL_TCP 6
struct __attribute__((packed)) IPPacket
{
    uint8_t ihl:4;
    uint8_t version:4;
    uint8_t tos;
    uint16_t length;
    uint16_t id;
    uint8_t fragmentOffsetHigh:5;
    uint8_t flags:3;
    uint8_t fragmentOffsetLow;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t headerChecksum;
    uint8_t srcIP[4];
    uint8_t dstIP[4];
    uint8_t data[];
};
void ipSendPacket(uint8_t* destIP, void* data, size_t len, uint8_t protocol,
    EthernetDevice* dev);
void ipHandlePacket(IPPacket* packet, EthernetDevice* dev);
#endif
#endif