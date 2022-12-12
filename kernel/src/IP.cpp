#include <IP.h>
#include <ARP.h>
#include <Ethernet.h>
#include <UDP.h>
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
    packet->headerChecksum = ntohs(ip_calculate_checksum(packet));
    int counter = 3;
    qemu_printf("Sending IP Packet to %d.%d.%d.%d\n", destIP[0], destIP[1], destIP[2], destIP[3]);
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
    qemu_printf("IP packet recieved\n");
    packet->versionIHLPtr[0] = flipByte(packet->versionIHLPtr[0], 4);
    packet->flagsFragmentPtr[0] = flipByte(packet->flagsFragmentPtr[0], 3);
    if (packet->protocol == PROTOCOL_UDP)
    {
        udpHandlePacket((UDPPacket*)packet->data, dev);
    }
}