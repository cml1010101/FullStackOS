#include <RTL8139.h>
#include <MMU.h>
#include <IRQ.h>
#include <Ethernet.h>
extern PageDirectory* kernelDirectory;
RTL8139* defaultRTL8139;
Package tmpPackage;
#define RX_BUF_SIZE 8192
#define CAPR 0x38
#define RX_READ_POINTER_MASK (~3)
uint32_t previousIdx;
void RTL8139_interrupt(CPURegisters* regs)
{
    defaultRTL8139->handle();
}
uint32_t RTL8139::readl(uint32_t reg)
{
    return inl(ioBase + reg);
}
void RTL8139::writel(uint32_t reg, uint32_t val)
{
    outl(ioBase + reg, val);
}
uint16_t RTL8139::reads(uint32_t reg)
{
    return inw(ioBase + reg);
}
void RTL8139::writes(uint32_t reg, uint16_t val)
{
    outw(ioBase + reg, val);
}
uint8_t RTL8139::readb(uint32_t reg)
{
    return inb(ioBase + reg);
}
void RTL8139::writeb(uint32_t reg, uint8_t val)
{
    outb(ioBase + reg, val);
}
void RTL8139::handle()
{
    uint16_t isr = reads(RTL_ISR);
    if (isr & RTL_TOK)
    {
        writes(RTL_ISR, RTL_TOK);
        while (readl(RTL_TSD_BASE + (finishDescriptor * 4)) & (RTL_TSD_OWN | RTL_TSD_TOK)
            == RTL_TSD_BOTH && freeDescriptors < 4)
        {
            finishDescriptor = (finishDescriptor + 1) % 4;
            freeDescriptors;
        }
    }
    if (isr & RTL_ROK)
    {
        do
        {
            uint8_t* rxPointer = (uint8_t*)(recieveBuffer + rxOffset);
            uint16_t packetLength = *(uint16_t*)(rxPointer + 2);
            writes(RTL_ISR, RTL_ROK);
            uint8_t* packetBuffer = new uint8_t[2048];
            if ((uint64_t)rxPointer + packetLength >= recieveBuffer + RTL8139_RXBUFFER_SIZE)
            {
                uint64_t first_run = (recieveBuffer + RTL8139_RXBUFFER_SIZE) - (uint64_t)rxPointer;
                memcpy(packetBuffer, rxPointer, first_run);
                memcpy(packetBuffer + first_run, (void*)recieveBuffer, packetLength - first_run);
            }
            else
            {
                memcpy(packetBuffer, rxPointer, packetLength);
            }
            rxOffset = (rxOffset + packetLength + 4 + 3) & ~0x3;
		    rxOffset %= RTL8139_RXBUFFER_SIZE;
            writes(RTL_CAPR, rxOffset - 0x10);
            recieveEthernetPacket((EthernetFrame*)(packetBuffer + 4), packetLength, this);
        }
        while (!(readb(RTL_CR) & RTL_BUFE));
    }
}
RTL8139::RTL8139(PCIDevice* dev)
{
    defaultRTL8139 = this;
    for (size_t i = 0; i < 6; i++)
        if (!(dev->getBar(i) & 1))
        {
            mmioBase = dev->getBar(i) & ~7;
            break;
        }
    ioBase = dev->getBar(0) & 0xFFFC;
    registerIRQHandler(10, RTL8139_interrupt);
    dev->writeCommand(dev->getCommand() | (1 << 2));
    kernelDirectory->mapPage(mmioBase, mmioBase, MMU_RW | MMU_PRESENT);
    switchDirectory(kernelDirectory);
    memcpy(macAddress, (void*)mmioBase, 6);
#ifdef __DEBUG__
    qemu_printf("MMIO Base: 0x%x, Mac address: %x:%x:%x:%x:%x:%x\n", mmioBase,
        macAddress[0], macAddress[1],
        macAddress[2], macAddress[3],
        macAddress[4], macAddress[5]);
#endif
    for (size_t i = 0; i < 4; i++)
    {
        txDescs[i].buffer = (uint8_t*)(txDescs[i].buffer_phys = kmalloc_a(i));
        txDescs[i].packet_length = 0;
    }
    writeb(RTL_CONFIG1, 0);
    writeb(RTL_CR, 0x10);
    while (readb(RTL_CR) & 0x10);
    recieveBuffer = kmalloc_a(RTL8139_RXBUFFER_SIZE + 0x1000);
    memset((void*)recieveBuffer, 0, RTL8139_RXBUFFER_SIZE + 0x1000);
    writel(RTL_RBSTART, recieveBuffer);
    writes(RTL_IMR, 0x5);
    writel(RTL_RCR, 0xFF);
    writeb(RTL_CR, 0xC);
    rxOffset = 0;
    finishDescriptor = 0;
    freeDescriptors = 4;
    currentDescriptor = 0;
}
void RTL8139::sendPacket(Package* networkPackage)
{
    qemu_printf("Sending packet\n");
    while (freeDescriptors == 0);
    memcpy(txDescs[currentDescriptor].buffer, networkPackage->data, networkPackage->len);
    txDescs[currentDescriptor].packet_length = networkPackage->len;
    size_t prevDesc = currentDescriptor;
    currentDescriptor = (currentDescriptor + 1) % 4;
    freeDescriptors--;
    writel(RTL_TSAD_BASE + prevDesc * 4, txDescs[prevDesc].buffer_phys);
	writel(RTL_TSD_BASE + prevDesc* 4, (txDescs[prevDesc].packet_length & 0xfff)
        | (48 << 16));
}
Package* RTL8139::recievePacket()
{
    return NULL;
}
uint8_t* RTL8139::getMAC()
{
    return macAddress;
}