#ifndef DHCP_H
#define DHCP_H
#include <SmartOS.h>
#ifdef __cplusplus
struct __attribute__((packed)) DHCPPacket
{
    uint8_t op;
    uint8_t hardware_type;
    uint8_t hardware_addr_len;
    uint8_t hops;
    uint32_t xid;
    uint16_t seconds;
    uint16_t flags;
    uint32_t client_ip;
    uint32_t your_ip;
    uint32_t server_ip;
    uint32_t gateway_ip;
    uint8_t client_hardware_addr[16];
    uint8_t server_name[64];
    uint8_t file[128];
    uint8_t options[64];
};
void dhcpDiscover(EthernetDevice* dev);
void dhcpRequest(uint8_t* ip, EthernetDevice* dev);
void dhcpHandlePacket(DHCPPacket* packet, EthernetDevice* dev);
#endif
#endif