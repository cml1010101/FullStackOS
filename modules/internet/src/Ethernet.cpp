#include <Ethernet.h>
#include <RTL8139.h>
#include <E1000.h>
#include <MMU.h>
#include <ARP.h>
#include <DHCP.h>
#include <IP.h>
#include <TCP.h>
#include <Scheduler.h>
Vector<EthernetDevice*> ethernetDevices;
uint8_t sourceIP[4];
const uint8_t _broadcastMAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const uint8_t _broadcastIP4[4] = {0xFF, 0xFF, 0xFF, 0xFF};
void initializeEthernet()
{
    memset(sourceIP, 0x0, 4);
    ethernetDevices = {};
    initializeARP();
    initializeTCP();
    for (size_t i = 0; i < pciDevices.size(); i++)
    {
        if (pciDevices[i].getDeviceID() == 0x8139)
        {
            EthernetDevice* dev;
            ethernetDevices.push(dev = new RTL8139(&pciDevices[i]));
            dhcpDiscover(dev);
        }
        if (pciDevices[i].getDeviceID() == E1000_DEV)
        {
            EthernetDevice* dev;
            ethernetDevices.push(dev = new E1000(&pciDevices[i]));
            dhcpDiscover(dev);
        }
    }
}
void sendEthernetPacket(const uint8_t* dest, uint8_t* data, size_t len, uint16_t protocol,
    EthernetDevice* dev)
{
    EthernetFrame* frame = (EthernetFrame*)malloc(sizeof(EthernetFrame) + len);
    memcpy(frame->srcMac, dev->getMAC(), 6);
    memcpy(frame->destMac, dest, 6);
    memcpy(frame->data, data, len);
    frame->type = ntohs(protocol);
    Package* pkg = new Package;
    pkg->data = frame;
    pkg->len = len + sizeof(EthernetFrame);
    dev->sendPacket(pkg);
    free(pkg->data);
    free(pkg);
}
void recieveEthernetPacket(EthernetFrame* frame, size_t len, EthernetDevice* dev)
{
    if (ntohs(frame->type) == ETHERNET_TYPE_ARP)
    {
        arpHandlePacket((ARPPacket*)frame->data, len - sizeof(EthernetFrame), dev);
    }
    if (ntohs(frame->type) == ETHERNET_TYPE_IP4)
    {
        ipHandlePacket((IPPacket*)frame->data, dev);
    }
}
void setSourceIP(uint8_t* ip)
{
    memcpy(sourceIP, ip, 4);
}
uint8_t* getSourceIP()
{
    return sourceIP;
}