#include <IP.h>
<<<<<<< HEAD
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
=======
#include <ARP.h>
#include <Ethernet.h>
#define IP_IPV4 4
#define IP_PACKET_NO_FRAGMENT 2
#define IP_IS_LAST_FRAGMENT 4
#define PROTOCOL_UDP 17
#define PROTOCOL_TCP 6
const uint8_t zeroHardware[6] = {0, 0, 0, 0, 0, 0};
uint8_t flipByte(uint8_t byte, int num_bits)
{
    uint8_t t = byte << (8 - num_bits);
    return t | (byte >> num_bits);
}
uint16_t ip_calculate_checksum(IPPacket * packet) {
    int arraySize = sizeof(IPPacket) / 2;
    uint16_t* array = (uint16_t*)packet;
    uint8_t* array2 = (uint8_t*)packet;
    uint32_t sum = 0;
    for (int i = 0; i < arraySize; i++)
    {
>>>>>>> parent of 2c35866... fixes
        sum += ntohs(array[i]);
    }
    uint32_t carry = sum >> 16;
    sum = sum & 0x0000ffff;
    sum = sum + carry;
    uint16_t ret = ~sum;
    return ret;
}
<<<<<<< HEAD
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
=======
void ipSendPacket(uint8_t* destIP, void* data, size_t len, uint8_t protocol,
    EthernetDevice* dev)
{
    qemu_printf("Sending package\n");
    IPPacket* packet = (IPPacket*)malloc(sizeof(IPPacket) + len);
    memset(packet, 0, sizeof(IPPacket));
    packet->version = IP_IPV4;
    packet->ihl = 5;
    packet->tos = 0;
    packet->flags = 0;
    packet->ttl = 64;
    packet->id = 0;
    packet->length = len + sizeof(IPPacket);
    packet->fragmentOffsetHigh = 0;
    packet->fragmentOffsetLow = 0;
    packet->protocol = protocol;
    memcpy(packet->srcIP, getSourceIP(), 4);
    memcpy(packet->dstIP, destIP, 4);
    memcpy((void*)packet + packet->ihl * 4, data, len);
    packet->length = ntohs(len + sizeof(IPPacket));
    packet->versionIHLPtr[0] = flipByte(packet->versionIHLPtr[0], 4);
    packet->flagsFragmentPtr[0] = flipByte(packet->flagsFragmentPtr[0], 3);
    packet->headerChecksum = ip_calculate_checksum(packet);
    int counter = 3;
    qemu_printf("Sending IP Packet to %x.%x.%x.%x\n", destIP[0], destIP[1], destIP[2], destIP[3]);
    qemu_printf("ARP has? %s\n", arpHas(destIP) ? "true" : "false");
    while (!arpHas(destIP))
    {
        if (counter)
        {
            arpSendPacket(zeroHardware, destIP, dev);
            counter--;
        }
    }
    sendEthernetPacket(arpFind(destIP), (uint8_t*)packet, ntohs(packet->length),
        ETHERNET_TYPE_IP4, dev);
}
void ipHandlePacket(IPPacket* packet, EthernetDevice* dev)
{
    packet->versionIHLPtr[0] = flipByte(packet->versionIHLPtr[0], 4);
    packet->flagsFragmentPtr[0] = flipByte(packet->flagsFragmentPtr[0], 3);
>>>>>>> parent of 2c35866... fixes
}