#include <UDP.h>
#include <IP.h>
#include <DHCP.h>
uint16_t udpChecksum(UDPPacket * packet) {
    int arraySize = sizeof(UDPPacket) / 2;
    uint16_t* array = (uint16_t*)packet;
    uint8_t* array2 = (uint8_t*)packet;
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
void udpSendPacket(uint8_t* destIP, uint16_t srcPort, uint16_t destPort, void* data, size_t len,
    EthernetDevice* dev)
{
    UDPPacket* packet = (UDPPacket*)malloc(len);
    memset(packet, 0, sizeof(UDPPacket) + len);
    packet->srcPort = ntohs(srcPort);
    packet->destPort = ntohs(destPort);
    packet->length = ntohs(len);
    packet->checksum = 0;
    packet->checksum = udpChecksum(packet);
    memcpy(packet->data, data, len);
    ipSendPacket(destIP, packet, len, PROTOCOL_UDP, dev);
}
void udpHandlePacket(UDPPacket* packet, EthernetDevice* dev)
{
    uint16_t destPort = ntohs(packet->destPort);
    uint16_t length = ntohs(packet->length);
    if (ntohs(packet->destPort) == 68)
    {
        dhcpHandlePacket((DHCPPacket*)packet->data, dev);
    }
}