#ifndef ARP_H
#define ARP_H
#include <SmartOS.h>
#ifdef __cplusplus
struct __attribute__((packed)) ARPPacket
{
    uint16_t hardwareType;
    uint16_t protocol;
    uint8_t hardwareAddrLen;
    uint8_t protocolAddrLen;
    uint16_t opcode;
    uint8_t srcHardwareAddr[6];
    uint8_t srcProtocolAddr[4];
    uint8_t dstHardwareAddr[6];
    uint8_t dstProtocolAddr[4];
};
struct __attribute__((packed)) ARPEntry
{
    uint8_t ip4[4];
    uint8_t mac[6];
    ARPEntry() = default;
    inline ARPEntry(const uint8_t* ip4, const uint8_t* mac)
    {
        memcpy(this->ip4, ip4, 4);
        memcpy(this->mac, mac, 6);
    }
};
void arpHandlePacket(ARPPacket* packet, size_t len, EthernetDevice* dev);
void arpSendPacket(const uint8_t* destHardware, uint8_t* destProtocol, EthernetDevice* dev);
uint8_t* arpFind(uint8_t* ip);
bool arpHas(uint8_t* ip);
void initializeARP();
#endif
#endif