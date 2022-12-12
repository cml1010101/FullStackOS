#include <DHCP.h>
#include <Ethernet.h>
#include <UDP.h>
#define DHCP_REQUEST 1
#define DHCP_REPLY 2
#define DHCP_TRANSACTION_IDENTIFIER 0x55555555
void makeDHCPPacket(DHCPPacket* packet, uint8_t msgType, uint8_t* ip,
    EthernetDevice* dev)
{
    packet->op = DHCP_REQUEST;
    packet->hardware_type = HARDWARE_TYPE_ETHERNET;
    packet->hardware_addr_len = 6;
    packet->hops = 0;
    packet->xid = 0;
    packet->flags = ntohs(0x8000);
    memcpy(packet->client_hardware_addr, dev->getMAC(), 6);
    uint8_t dst_ip[4];
    memset(dst_ip, 0xff, 4);
    uint8_t* options = packet->options;
    *((uint32_t*)(options)) = htonl(0x63825363);
    options += 4;
    *(options++) = 53;
    *(options++) = 1;
    *(options++) = msgType;
    *(options++) = 61;
    *(options++) = 0x07;
    *(options++) = 0x01;
    memcpy(options, dev->getMAC(), 6);
    options += 6;
    *(options++) = 50;
    *(options++) = 0x04;
    *((uint32_t*)(options)) = htonl(0x0a00020e);
    memcpy((uint32_t*)(options), ip, 4);
    options += 4;
    *(options++) = 12;
    *(options++) = 0x08;
    memcpy(options, "smartos", strlen("smartos"));
    options += strlen("smartos");
    *(options++) = 0x00;
    *(options++) = 55;
    *(options++) = 8;
    *(options++) = 0x1;
    *(options++) = 0x3;
    *(options++) = 0x6;
    *(options++) = 0xf;
    *(options++) = 0x2c;
    *(options++) = 0x2e;
    *(options++) = 0x2f;
    *(options++) = 0x39;
    *(options++) = 0xff;
}
void dhcpDiscover(EthernetDevice* dev)
{
    uint8_t requestIP[4];
    uint8_t destIP[4];
    memset(requestIP, 0x0, 4);
    memset(destIP, 0xff, 4);
    DHCPPacket* packet = new DHCPPacket;
    memset(packet, 0, sizeof(DHCPPacket));
    makeDHCPPacket(packet, 1, requestIP, dev);
    udpSendPacket(destIP, 68, 67, packet, sizeof(DHCPPacket), dev);
}
void dhcpRequest(uint8_t* ip, EthernetDevice* dev)
{
    uint8_t destIP[4];
    memset(destIP, 0xFF, 4);
    DHCPPacket* packet = new DHCPPacket;
    memset(packet, 0, sizeof(DHCPPacket));
    makeDHCPPacket(packet, 3, ip, dev);
    udpSendPacket(destIP, 68, 67, packet, sizeof(DHCPPacket), dev);
}
void* getDHCPOptions(DHCPPacket* packet, uint8_t type) {
    uint8_t * options = packet->options + 4;
    uint8_t curr_type = *options;
    while(curr_type != 0xff)
    {
        uint8_t len = *(options + 1);
        if(curr_type == type)
        {
            void* ret = malloc(len);
            memcpy(ret, options + 2, len);
            return ret;
        }
        options += (2 + len);
    }
    return NULL;
}
void dhcpHandlePacket(DHCPPacket* packet, EthernetDevice* dev)
{
    uint8_t* options = packet->options + 4;
    if (packet->op == DHCP_REPLY)
    {
        uint8_t* type = (uint8_t*)getDHCPOptions(packet, 53);
        qemu_printf("%x\n", type);
        if(*type == 2)
        {
            dhcpRequest((uint8_t*)&packet->your_ip, dev);
        }
        else if (*type == 5)
        {
            setSourceIP((uint8_t*)&packet->your_ip);
        }
    }
}