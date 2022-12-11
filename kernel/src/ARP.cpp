#include <ARP.h>
#include <Ethernet.h>
Vector<InternetAddr> arpTable;
void arpHandler(const void* ptr, size_t len, EthernetDevice* dev)
{
    ARPPacket* packet = (ARPPacket*)ptr;
    if (ntohs(packet->opcode) == 1)
    {
        if (packet->protocolLength == 8)
        {
            ARPPacket* newPacket = (ARPPacket*)malloc(sizeof(ARPPacket) + 28);
            newPacket->opcode = ntohs(2);
            newPacket->hardwareLength = 6;
            newPacket->protocolLength = 8;
            newPacket->hardwareType = ntohs(HARDWARE_TYPE_ETHERNET);
            newPacket->protocol = ntohs(ETHERNET_TYPE_IP6);
            memcpy(newPacket->data, dev->getMAC(), 6);
            memcpy(newPacket->data + 6, getSourceIP6(), 8);
            memcpy(newPacket->data + 14, packet->data, 6);
            memcpy(newPacket->data + 20, packet->data + 6, 8);
            sendEthernetPacket(packet->data, (uint8_t*)newPacket, sizeof(ARPPacket) + 28, ARP_TYPE,
                dev);
        }
        else
        {
            ARPPacket* newPacket = (ARPPacket*)malloc(sizeof(ARPPacket) + 20);
            newPacket->opcode = ntohs(2);
            newPacket->hardwareLength = 6;
            newPacket->protocolLength = 4;
            newPacket->hardwareType = ntohs(HARDWARE_TYPE_ETHERNET);
            newPacket->protocol = ntohs(ETHERNET_TYPE_IP4);
            memcpy(newPacket->data, dev->getMAC(), 6);
            memcpy(newPacket->data + 6, getSourceIP6(), 4);
            memcpy(newPacket->data + 10, packet->data, 6);
            memcpy(newPacket->data + 26, packet->data + 6, 4);
            sendEthernetPacket(packet->data, (uint8_t*)newPacket, sizeof(ARPPacket) + 20, ARP_TYPE,
                dev);
        }
    }
    if (packet->protocolLength == 8)
    {
        for (size_t i = 0; i < arpTable.size(); i++)
        {
            if (memcmp(arpTable[i].ip6, packet->data + 6, 8) == 0)
            {
                memcpy(arpTable[i].mac, packet->data, 6);
                return;
            }
        }
        InternetAddr addr;
        memset(&addr, 0, sizeof(InternetAddr));
        memcpy(addr.mac, packet->data, 6);
        memcpy(addr.ip6, packet->data + 6, 8);
    }
    else
    {
        for (size_t i = 0; i < arpTable.size(); i++)
        {
            if (memcmp(arpTable[i].ip4, packet->data + 6, 4) == 0)
            {
                memcpy(arpTable[i].mac, packet->data, 6);
                return;
            }
        }
        InternetAddr addr;
        memset(&addr, 0, sizeof(InternetAddr));
        memcpy(addr.mac, packet->data, 6);
        memcpy(addr.ip4, packet->data + 6, 4);
    }
}
void initializeARP()
{
    arpTable = {};
    addEthernetProtocol(NULL, {ARP_TYPE, arpHandler});
}
InternetAddr* getInternetAddrIPV4(const uint8_t* ip4)
{
    for (size_t i = 0; i < arpTable.size(); i++)
    {
        if (memcmp(arpTable[i].ip4, ip4, 4) == 0)
        {
            return &arpTable[i];
        }
    }
    return NULL;
}
InternetAddr* getInternetAddrIPV6(const uint8_t* ip6)
{
    for (size_t i = 0; i < arpTable.size(); i++)
    {
        if (memcmp(arpTable[i].ip6, ip6, 8) == 0)
        {
            return &arpTable[i];
        }
    }
    return NULL;
}