#include <IP.h>
#include <ARP.h>
#include <Ethernet.h>
#include <UDP.h>
#include <TCP.h>
#define IP_IPV4 4
#define IP_PACKET_NO_FRAGMENT 2
#define IP_IS_LAST_FRAGMENT 4
#define PROTOCOL_UDP 17
#define PROTOCOL_TCP 6
extern Vector<ARPEntry> arpTable;
const uint8_t zeroHardware[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t flipByte(uint8_t byte, int num_bits)
{
    uint8_t t = byte << (8 - num_bits);
    return t | (byte >> num_bits);
}
uint16_t ip_calculate_checksum(IPPacket * packet) {
    int arraySize = sizeof(IPPacket) / 2;
    uint16_t* array = (uint16_t*)packet;
    uint32_t sum = 0;
    for (int i = 0; i < arraySize; i++)
    {
        sum += ntohs(array[i]);
    }
    uint32_t carry = sum >> 16;
    sum = sum & 0x0000ffff;
    sum = sum + carry;
    uint16_t ret = ~sum;
    return ret;
}
void ipSendPacket(uint8_t* destIP, void* data, size_t len, uint8_t protocol,
    EthernetDevice* dev)
{
    IPPacket* packet = (IPPacket*)malloc(sizeof(IPPacket) + len);
    memset(packet, 0, sizeof(IPPacket));
    packet->version = IP_IPV4;
    packet->ihl = 5;
    packet->tos = 0;
    packet->flags = 0;
    packet->ttl = 64;
    packet->id = 0;
    packet->fragmentOffsetHigh = 0;
    packet->fragmentOffsetLow = 0;
    packet->protocol = protocol;
    memcpy(packet->srcIP, getSourceIP(), 4);
    memcpy(packet->dstIP, destIP, 4);
    memcpy((uint8_t*)packet + sizeof(IPPacket), data, len);
    packet->length = ntohs(len + sizeof(IPPacket));
    packet->headerChecksum = ntohs(ip_calculate_checksum(packet));
    asm volatile ("sti");
    if (!arpHas(destIP)) arpSendPacket(zeroHardware, destIP, dev);
    while (!arpHas(destIP));
    sendEthernetPacket(arpFind(destIP), (uint8_t*)packet, len + sizeof(IPPacket),
        ETHERNET_TYPE_IP4, dev);
    free(packet);
}
void ipHandlePacket(IPPacket* packet, EthernetDevice* dev)
{
    if (packet->protocol == PROTOCOL_UDP)
    {
        udpHandlePacket((UDPPacket*)packet->data, dev);
    }
    if (packet->protocol == PROTOCOL_TCP)
    {
        tcpRecieve((TCPHeader*)packet->data, packet->srcIP, ntohs(packet->length)
            - sizeof(IPPacket), dev);
    }
}