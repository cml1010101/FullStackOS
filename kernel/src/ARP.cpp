#include <ARP.h>
#include <Ethernet.h>
Vector<InternetAddr> arpTable;
void arpHandler(const void* ptr, size_t len, EthernetDevice* dev)
{
    ARPPacket* packet = (ARPPacket*)ptr;
    if (ntohs(packet->opcode) == 1)
    {
        if (packet->protocolLength == 16)
        {
            ARPPacket* newPacket = (ARPPacket*)malloc(sizeof(ARPPacket) + 44);
            newPacket->opcode = ntohs(2);
            newPacket->hardwareLength = 6;
            newPacket->protocolLength = 16;
            newPacket->hardwareType = ntohs(HARDWARE_TYPE_ETHERNET);
            newPacket->protocol = ntohs(ETHERNET_TYPE_IP6);
            memcpy(newPacket->data, dev->getMAC(), 6);
            memcpy(newPacket->data + 6, getSourceIP6(), 16);
            memcpy(newPacket->data + 22, packet->data, 6);
            memcpy(newPacket->data + 28, packet->data + 6, 16);
            sendEthernetPacket(packet->data, (uint8_t*)newPacket, sizeof(ARPPacket) + 44, ARP_TYPE,
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
            memcpy(newPacket->data + 16, packet->data + 6, 4);
            sendEthernetPacket(packet->data, (uint8_t*)newPacket, sizeof(ARPPacket) + 20, ARP_TYPE,
                dev);
        }
    }
    if (packet->protocolLength == 16)
    {
        for (size_t i = 0; i < arpTable.size(); i++)
        {
            if (memcmp(arpTable[i].ip6, packet->data + 6, 16) == 0)
            {
                memcpy(arpTable[i].mac, packet->data, 6);
                return;
            }
        }
        InternetAddr addr;
        memset(&addr, 0, sizeof(InternetAddr));
        memcpy(addr.mac, packet->data, 6);
        memcpy(addr.ip6, packet->data + 6, 16);
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
void arpSearchIPV4(const uint8_t* ip4, EthernetDevice* dev)
{
    uint8_t broadcastMAC[6];
    memset(broadcastMAC, 0xFF, 6);
    ARPPacket* newPacket = (ARPPacket*)malloc(sizeof(ARPPacket) + 20);
    newPacket->opcode = ntohs(1);
    newPacket->hardwareLength = 6;
    newPacket->protocolLength = 4;
    newPacket->hardwareType = ntohs(HARDWARE_TYPE_ETHERNET);
    newPacket->protocol = ntohs(ETHERNET_TYPE_IP4);
    memcpy(newPacket->data, dev->getMAC(), 6);
    memcpy(newPacket->data + 6, getSourceIP6(), 4);
    memcpy(newPacket->data + 10, broadcastMAC, 6);
    memcpy(newPacket->data + 26, ip4, 4);
    sendEthernetPacket(broadcastMAC, (uint8_t*)newPacket, sizeof(ARPPacket) + 20, ARP_TYPE,
        dev);
}
void arpSearchIPV6(const uint8_t* ip6, EthernetDevice* dev)
{
    uint8_t broadcastMAC[6];
    memset(broadcastMAC, 0xFF, 6);
    ARPPacket* newPacket = (ARPPacket*)malloc(sizeof(ARPPacket) + 44);
    newPacket->opcode = ntohs(1);
    newPacket->hardwareLength = 6;
    newPacket->protocolLength = 4;
    newPacket->hardwareType = ntohs(HARDWARE_TYPE_ETHERNET);
    newPacket->protocol = ntohs(ETHERNET_TYPE_IP4);
    memcpy(newPacket->data, dev->getMAC(), 6);
    memcpy(newPacket->data + 6, getSourceIP6(), 16);
    memcpy(newPacket->data + 22, broadcastMAC, 6);
    memcpy(newPacket->data + 28, ip6, 16);
    sendEthernetPacket(broadcastMAC, (uint8_t*)newPacket, sizeof(ARPPacket) + 44, ARP_TYPE,
        dev);
}
void initializeARP()
{
    arpTable = {};
    InternetAddr broadcastAddr;
    memset(&broadcastAddr, 0xFF, sizeof(InternetAddr));
    arpTable.push(broadcastAddr);
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
        if (memcmp(arpTable[i].ip6, ip6, 16) == 0)
        {
            return &arpTable[i];
        }
    }
    return NULL;
}