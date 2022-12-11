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
        if (memcmp(arpTable[i].ip4, ip, 4) == 0)
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