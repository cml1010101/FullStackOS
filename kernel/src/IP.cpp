#include <IP.h>
#include <Ethernet.h>
#include <UDP.h>
#include <ARP.h>
#define PROTOCOL_UDP 17
#define PROTOCOL_TCP 6
uint16_t ipChecksum(IPV4Packet* packet)
{
    int array_size = sizeof(IPV4Packet) / 2;
    uint16_t * array = (uint16_t*)packet;
    uint8_t * array2 = (uint8_t*)packet;
    uint32_t sum = 0;
    for(int i = 0; i < array_size; i++) {
        sum += ntohs(array[i]);
    }
    uint32_t carry = sum >> 16;
    sum = sum & 0x0000ffff;
    sum = sum + carry;
    uint16_t ret = ~sum;
    return ret;
}
uint16_t ipChecksum(IPV6Packet* packet)
{
    int array_size = sizeof(IPV6Packet) / 2;
    uint16_t * array = (uint16_t*)packet;
    uint8_t * array2 = (uint8_t*)packet;
    uint32_t sum = 0;
    for(int i = 0; i < array_size; i++) {
        sum += ntohs(array[i]);
    }
    uint32_t carry = sum >> 16;
    sum = sum & 0x0000ffff;
    sum = sum + carry;
    uint16_t ret = ~sum;
    return ret;
}
void ipv4Handler(const void* ptr, size_t len, EthernetDevice* dev)
{
    IPV4Packet* packet = (IPV4Packet*)ptr;
    if (packet->protocol == PROTOCOL_UDP)
    {
        const void* data = ptr + sizeof(IPV4Packet);
        size_t dataLength = ntohs(packet->length) - sizeof(IPV4Packet);
        qemu_printf("Recieved IP Packet\n");
        udpHandle(data, dataLength, dev);
    }
}
void ipv6Handler(const void* ptr, size_t len, EthernetDevice* dev)
{
    IPV6Packet* packet = (IPV6Packet*)ptr;
    if (packet->protocol == PROTOCOL_UDP)
    {
        const void* data = ptr + sizeof(IPV6Packet);
        size_t dataLength = ntohs(packet->length) - sizeof(IPV6Packet);
        qemu_printf("Recieved IP Packet\n");
        udpHandle(data, dataLength, dev);
    }
}
void initializeIP()
{
    addEthernetProtocol(NULL, {ETHERNET_TYPE_IP4, ipv4Handler});
    addEthernetProtocol(NULL, {ETHERNET_TYPE_IP6, ipv6Handler});
}
void ipv4Send(const uint8_t* ip4, const void* data, size_t len, EthernetDevice* dev)
{
    IPV4Packet* packet = (IPV4Packet*)malloc(sizeof(IPV4Packet) + len);
    memset(packet, 0, sizeof(IPV4Packet));
    packet->version = 4;
    packet->ihl = 5;
    packet->service = 0;
    packet->id = 0;
    packet->flags = 0;
    packet->ttl = 64;
    packet->length = len + sizeof(IPV6Packet);
    packet->protocol = PROTOCOL_UDP;
    memcpy(packet->srcAddress, getSourceIP4(), 4);
    memcpy(packet->destAddress, ip4, 4);
    packet->checksum = ipChecksum(packet);
    memcpy(&packet[1], data, len);
    InternetAddr* addr;
    int counter = 3;
    while (!(addr = getInternetAddrIPV4(ip4)))
    {
        if (counter--) arpSearchIPV4(ip4, dev);
    }
    sendEthernetPacket(addr->mac, (uint8_t*)packet, len + sizeof(IPV4Packet), ETHERNET_TYPE_IP4, dev);
}
void ipv6Send(const uint8_t* ip6, const void* data, size_t len, EthernetDevice* dev)
{
    IPV6Packet* packet = (IPV6Packet*)malloc(sizeof(IPV4Packet) + len);
    memset(packet, 0, sizeof(IPV6Packet));
    packet->version = 4;
    packet->ihl = 11;
    packet->service = 0;
    packet->id = 0;
    packet->flags = 0;
    packet->ttl = 64;
    packet->length = len + sizeof(IPV6Packet);
    packet->protocol = PROTOCOL_UDP;
    memcpy(packet->srcAddress, getSourceIP6(), 16);
    memcpy(packet->destAddress, ip6, 16);
    packet->checksum = ipChecksum(packet);
    memcpy(&packet[1], data, len);
    InternetAddr* addr;
    int counter = 3;
    while (!(addr = getInternetAddrIPV6(ip6)))
    {
        if (counter--) arpSearchIPV4(ip6, dev);
    }
    sendEthernetPacket(addr->mac, (uint8_t*)packet, len + sizeof(IPV4Packet), ETHERNET_TYPE_IP6, dev);
}