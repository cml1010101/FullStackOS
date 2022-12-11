#include <RTL8139.h>
#include <MMU.h>
#include <IRQ.h>
#include <Ethernet.h>
enum RTL8139_registers {
    MAC0             = 0x00,       // Ethernet hardware address
    MAR0             = 0x08,       // Multicast filter
    TxStatus0        = 0x10,       // Transmit status (Four 32bit registers)
    TxAddr0          = 0x20,       // Tx descriptors (also four 32bit)
    RxBuf            = 0x30, 
    RxEarlyCnt       = 0x34, 
    RxEarlyStatus    = 0x36,
    ChipCmd          = 0x37,
    RxBufPtr         = 0x38,
    RxBufAddr        = 0x3A,
    IntrMask         = 0x3C,
    IntrStatus       = 0x3E,
    TxConfig         = 0x40,
    RxConfig         = 0x44,
    Timer            = 0x48,        // A general-purpose counter
    RxMissed         = 0x4C,        // 24 bits valid, write clears
    Cfg9346          = 0x50, 
    Config0          = 0x51, 
    Config1          = 0x52,
    FlashReg         = 0x54, 
    GPPinData        = 0x58, 
    GPPinDir         = 0x59, 
    MII_SMI          = 0x5A, 
    HltClk           = 0x5B,
    MultiIntr        = 0x5C, 
    TxSummary        = 0x60,
    MII_BMCR         = 0x62, 
    MII_BMSR         = 0x64, 
    NWayAdvert       = 0x66, 
    NWayLPAR         = 0x68,
    NWayExpansion    = 0x6A,
    FIFOTMS          = 0x70,
    CSCR             = 0x74,
    PARA78           = 0x78, 
    PARA7c           = 0x7c,
};
RTL8139* defaultRTL8139;
Package tmpPackage;
#define RX_BUF_SIZE 8192
#define CAPR 0x38
#define RX_READ_POINTER_MASK (~3)
void RTL8139_interrupt(CPURegisters* regs)
{
    uint16_t status = inw(defaultRTL8139->ioBase + 0x3E);
    if (status & 1)
    {
        uint16_t * t = (uint16_t*)(defaultRTL8139->rxDescs + defaultRTL8139->rxPtr);
        uint16_t packet_length = *(t + 1);
        t = t + 2;
        void * packet = (void*)kmalloc_a(packet_length);
        memcpy(packet, t, packet_length);
        recieveEthernetPacket((EthernetFrame*)packet, packet_length, defaultRTL8139);
        defaultRTL8139->rxPtr = (defaultRTL8139->rxPtr + packet_length + 4 + 3) & RX_READ_POINTER_MASK;
        if(defaultRTL8139->rxPtr > RX_BUF_SIZE) defaultRTL8139->rxPtr -= RX_BUF_SIZE;
        outw(defaultRTL8139->ioBase + 0x38, defaultRTL8139->rxPtr - 0x10);
    }
    if (status & 4)
    {
        qemu_printf("Package sent\n");
    }
    outw(defaultRTL8139->ioBase + 0x3E, 5);
}
RTL8139::RTL8139(PCIDevice* dev)
{
    defaultRTL8139 = this;
    ioBase = dev->getBar(0) & 0xFFFC;
    registerIRQHandler(10, RTL8139_interrupt);
    dev->writeCommand(dev->getCommand() | (1 << 2));
    outb(ioBase + 0x52, 0);
    outb(ioBase + 0x37, 0x10);
    while (inb(ioBase + 0x37) & 0x10);
    rxDescs = (void*)kmalloc_a(8096 + 16);
    outl(ioBase + 0x30, (uint64_t)rxDescs);
    outw(ioBase + 0x3C, 5);
    outl(ioBase + 0x44, 0xFF);
    outb(ioBase + 0x37, 0xC);
    for (size_t i = 0; i < 6; i++) macAddress[i] = inb(ioBase + i);
#ifdef __DEBUG__
    qemu_printf("Mac address: %x %x %x %x %x %x\n",
        macAddress[0], macAddress[1],
        macAddress[2], macAddress[3],
        macAddress[4], macAddress[5]);
#endif
    rxPtr = 0;
    packetIdx = 0;
}
void RTL8139::sendPacket(Package* networkPackage)
{
    outl(ioBase + 0x20 + packetIdx * 4, (uint64_t)networkPackage->data);
    outl(ioBase + 0x10 + packetIdx * 4, networkPackage->len);
    packetIdx = (packetIdx + 1) % 4;
}
Package* RTL8139::recievePacket()
{
    packageACK = 0;
    while (!packageACK)
    {
        Package* pkg = new Package;
        pkg->data = tmpPackage.data;
        pkg->len = tmpPackage.len;
        return pkg;
    }
    return NULL;
}
uint8_t* RTL8139::getMAC()
{
    return macAddress;
}