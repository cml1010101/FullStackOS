#include <Ethernet.h>
#include <RTL8139.h>
#include <E1000.h>
#include <MMU.h>
Vector<EthernetDevice*> ethernetDevices;
Vector<ProtocolInit> protocolInits;
Vector<ProtocolHandler> protocolHandlers;
InternetAddr sourceAddr;
const uint8_t _broadcastMAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const uint8_t _broadcastIP4[4] = {0xFF, 0xFF, 0xFF, 0xFF};
void initializeEthernet()
{
    memset(&sourceAddr, 0x0, sizeof(sourceAddr));
    ethernetDevices = {};
    protocolInits = {};
    protocolHandlers = {};
}
void addEthernetProtocol(ProtocolInit initFunc, ProtocolHandler handlerFunc)
{
    if (initFunc != NULL) protocolInits.push(initFunc);
    if (handlerFunc.func != NULL) protocolHandlers.push(handlerFunc);
}
void addNIC(PCIDevice* dev)
{
    if (dev->getDeviceID() == 0x8139)
    {
        EthernetDevice* device;
        ethernetDevices.push(device = new RTL8139(dev));
        for (size_t i = 0; i < protocolInits.size(); i++)
        {
            protocolInits[i](device);
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
    Package pkg;
    pkg.data = frame;
    pkg.len = len + sizeof(EthernetFrame);
    dev->sendPacket(&pkg);
}
void recieveEthernetPacket(EthernetFrame* frame, size_t len, EthernetDevice* dev)
{
    for (size_t i = 0; i < protocolHandlers.size(); i++)
        if (ntohs(frame->type) == protocolHandlers[i].type)
            protocolHandlers[i].func(frame->data, len - sizeof(EthernetFrame), dev);
}
void setSourceIP4(uint8_t* ip)
{
    memcpy(sourceAddr.ip4, ip, 4);
}
void setSourceIP6(uint8_t* ip)
{
    memcpy(sourceAddr.ip6, ip, 8);
}
uint8_t* getSourceIP4()
{
    return sourceAddr.ip4;
}
uint8_t* getSourceIP6()
{
    return sourceAddr.ip6;
}