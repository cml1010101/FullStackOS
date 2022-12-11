#ifndef IP_H
#define IP_H
#include <SmartOS.h>
#ifdef __cplusplus
<<<<<<< HEAD
struct __attribute__((packed)) IPV4Packet
{
    uint8_t ihl: 4;
    uint8_t version: 4;
    uint8_t service;
    uint16_t length;
    uint16_t id;
    uint16_t flags;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint8_t srcAddress[4];
    uint8_t destAddress[4];
};
struct __attribute__((packed)) IPV6Packet
{
    uint8_t ihl: 4;
    uint8_t version: 4;
    uint8_t service;
    uint16_t length;
    uint16_t id;
    uint16_t flags;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint8_t srcAddress[16];
    uint8_t destAddress[16];
};
void initializeIP();
void ipv4Send(const uint8_t* ip4, const void* data, size_t len, EthernetDevice* dev);
void ipv6Send(const uint8_t* ip6, const void* data, size_t len, EthernetDevice* dev);
=======
#define PROTOCOL_UDP 17
#define PROTOCOL_TCP 6
struct __attribute__((packed)) IPPacket
{
    char versionIHLPtr[0];
    uint8_t version:4;
    uint8_t ihl:4;
    uint8_t tos;
    uint16_t length;
    uint16_t id;
    char flagsFragmentPtr[0];
    uint8_t flags:3;
    uint8_t fragmentOffsetHigh:5;
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
>>>>>>> parent of 2c35866... fixes
#endif
#endif