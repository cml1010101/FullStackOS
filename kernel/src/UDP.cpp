#include <UDP.h>
void udpHandle(const void* data, size_t length, EthernetDevice* dev)
{
}
void udpSend(InternetAddr* addr, uint16_t srcPort, uint16_t destPort, const void* data, size_t len,
    EthernetDevice* dev)
{
    UDPPacket* packet = (UDPPacket*)malloc(sizeof(UDPPacket) + len);
    packet->src_port = ntohs(srcPort);
    packet->dst_port = ntohs(destPort);
    packet->checksum = 0;
    packet->length = len;
    memcpy(packet->data, data, len);
}