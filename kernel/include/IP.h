#ifndef IP_H
#define IP_H
#include <SmartOS.h>
#ifdef __cplusplus
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
#endif
#endif