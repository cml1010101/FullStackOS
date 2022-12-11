#include <ARP.h>
#include <Ethernet.h>
#define ARP_REQUEST 1
#define ARP_REPLY 2
Vector<ARPEntry> arpTable;
const uint8_t broadcastMAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const uint8_t broadcastIP4[4] = {0xFF, 0xFF, 0xFF, 0xFF};
void arpHandlePacket(ARPPacket* packet, size_t len, EthernetDevice* dev)
{
    uint8_t destHardwareAddr[6];
    uint8_t destProtocolAddr[4];
    memcpy(destHardwareAddr, packet->srcHardwareAddr, 6);
    memcpy(destProtocolAddr, packet->srcProtocolAddr, 4);
    if (ntohs(packet->opcode) == ARP_REQUEST)
    {
<<<<<<< HEAD
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
=======
        uint32_t myIP = 0x0e02000a;
        if (memcmp(packet->dstProtocolAddr, &myIP, 4))
        {
            memcpy(packet->srcHardwareAddr, dev->getMAC(), 6);
            packet->srcProtocolAddr[0] = 10;
            packet->srcProtocolAddr[1] = 0;
            packet->srcProtocolAddr[2] = 2;
            packet->srcProtocolAddr[3] = 14;
            memcpy(packet->dstHardwareAddr, destHardwareAddr, 6);
            memcpy(packet->dstProtocolAddr, destProtocolAddr, 4);
            packet->opcode = ntohs(ARP_REPLY);
            packet->hardwareAddrLen = 6;
            packet->hardwareAddrLen = 4;
            packet->hardwareType = ntohs(HARDWARE_TYPE_ETHERNET);
            packet->protocol = ntohs(ETHERNET_TYPE_IP4);
            sendEthernetPacket(destHardwareAddr, (uint8_t*)packet, sizeof(ARPPacket),
                ETHERNET_TYPE_ARP, dev);
        }
    }
    arpTable.push({destHardwareAddr, destProtocolAddr});
}
void arpSendPacket(const uint8_t* destHardware, uint8_t* destProtocol, EthernetDevice* dev)
{
    qemu_printf("Sending arp packet\n");
    ARPPacket* packet = new ARPPacket;
    memcpy(packet->srcHardwareAddr, dev->getMAC(), 6);
    packet->srcProtocolAddr[0] = 10;
    packet->srcProtocolAddr[1] = 0;
    packet->srcProtocolAddr[2] = 2;
    packet->srcProtocolAddr[3] = 14;
    packet->hardwareType = ntohs(HARDWARE_TYPE_ETHERNET);
    packet->opcode = ntohs(ARP_REQUEST);
    packet->protocol = ntohs(ETHERNET_TYPE_IP4);
    memcpy(packet->dstHardwareAddr, destHardware, 6);
    memcpy(packet->dstProtocolAddr, destProtocol, 4);
    packet->hardwareAddrLen = 6;
    packet->protocolAddrLen = 4;
    sendEthernetPacket(broadcastMAC,
        (uint8_t*)packet, sizeof(ARPPacket),
        ETHERNET_TYPE_ARP, dev);
>>>>>>> parent of 2c35866... fixes
}
uint8_t* arpFind(uint8_t* ip)
{
    for (size_t i = 0; i < arpTable.size(); i++)
    {
        if (memcmp(arpTable[i].ip4, ip, 4) == 0)
        {
            return arpTable[i].mac;
        }
    }
    return NULL;
}
bool arpHas(uint8_t* ip)
{
    for (size_t i = 0; i < arpTable.size(); i++)
    {
<<<<<<< HEAD
        if (memcmp(arpTable[i].ip6, ip6, 16) == 0)
=======
        if (memcmp(arpTable[i].ip4, ip, 4) == 0)
>>>>>>> parent of 2c35866... fixes
        {
            return true;
        }
    }
    return false;
}
void initializeARP()
{
    arpTable = {};
    arpTable.push(ARPEntry(broadcastIP4, broadcastMAC));
}