#ifndef ARP_H
#define ARP_H
#include <SmartOS.h>
#define ARP_TYPE 0x0806
#ifdef __cplusplus
struct __attribute__((packed)) ARPPacket
{
    uint16_t hardwareType;
    uint16_t protocol;
    uint8_t hardwareLength;
    uint8_t protocolLength;
    uint16_t opcode;
    uint8_t data[];
};
void initializeARP();
InternetAddr* getInternetAddrIPV4(const uint8_t* ip4);
InternetAddr* getInternetAddrIPV6(const uint8_t* ip6);
#endif
#endif