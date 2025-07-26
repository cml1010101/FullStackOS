#include <E1000.h>
#include <MMU.h>
#include <IRQ.h>
#include <Ethernet.h>
extern PageDirectory* kernelDirectory;
E1000* defaultE1000;
void E1000::writeCommand(uint16_t addr, uint32_t val)
{
    if (ioBase) outl(ioBase + addr, val);
    else *(volatile uint32_t*)((uintptr_t)(mmioBase + addr)) = val;
}
uint32_t E1000::readCommand(uint16_t addr)
{
    if (ioBase) return inl(ioBase + addr);
    else return *(volatile uint32_t*)((uintptr_t)(mmioBase + addr));
}
bool E1000::detectEEProm()
{
    writeCommand(REG_EEPROM, 1);
    for (size_t i = 0; i < 1000 && !eepromExists; i++)
    {
        if (readCommand(REG_EEPROM) & 0x10) eepromExists = true;
        else eepromExists = false;
    }
    return eepromExists;
}
uint32_t E1000::eepromRead(uint8_t addr)
{
    uint32_t data = 0, tmp = 0;
    if (eepromExists)
    {
        writeCommand(REG_EEPROM, 1 | ((uint32_t)(addr) << 8));
        while(!((tmp = readCommand(REG_EEPROM)) & (1 << 4)));
    }
    else
    {
        writeCommand(REG_EEPROM, 1 | ((uint32_t)(addr) << 2));
        while(!((tmp = readCommand(REG_EEPROM)) & (1 << 1)));
    }
	data = (uint16_t)((tmp >> 16) & 0xFFFF);
	return data;
}
void E1000::rxInit()
{
    rxDescs = (E1000RXDescriptor*)kmalloc(E1000_NUM_RX_DESC * sizeof(E1000RXDescriptor) + 16);
    for (size_t i = 0; i < E1000_NUM_RX_DESC; i++)
    {
        rxDescs[i].addr = kmalloc(8196 + 16);
        rxDescs[i].status = 0;
    }
    writeCommand(REG_TXDESCLO, (uint32_t)((uint64_t)rxDescs >> 32));
    writeCommand(REG_TXDESCHI, (uint32_t)((uint64_t)rxDescs & 0xFFFFFFFF));
    writeCommand(REG_RXDESCLO, (uint64_t)rxDescs);
    writeCommand(REG_RXDESCHI, 0);
    writeCommand(REG_RXDESCLEN, E1000_NUM_RX_DESC * 16);
    writeCommand(REG_RXDESCHEAD, 0);
    writeCommand(REG_RXDESCTAIL, E1000_NUM_RX_DESC-1);
    rxIdx = 0;
    writeCommand(REG_RCTRL, RCTL_EN| RCTL_SBP| RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE 
        | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC | RCTL_BSIZE_8192);
}
void E1000::txInit()
{
    txDescs = (E1000TXDescriptor*)kmalloc(E1000_NUM_TX_DESC * sizeof(E1000TXDescriptor) + 16);
    for(int i = 0; i < E1000_NUM_TX_DESC; i++)
    {;
        txDescs[i].addr = 0;
        txDescs[i].cmd = 0;
        txDescs[i].status = TSTA_DD;
    }
    writeCommand(REG_TXDESCHI, (uint32_t)((uint64_t)txDescs >> 32) );
    writeCommand(REG_TXDESCLO, (uint32_t)((uint64_t)txDescs & 0xFFFFFFFF));
    writeCommand(REG_TXDESCLEN, E1000_NUM_TX_DESC * 16);
    writeCommand( REG_TXDESCHEAD, 0);
    writeCommand( REG_TXDESCTAIL, 0);
    txIdx = 0;
    writeCommand(REG_TCTRL, TCTL_EN
        | TCTL_PSP
        | (15 << TCTL_CT_SHIFT)
        | (64 << TCTL_COLD_SHIFT)
        | TCTL_RTLC);
    writeCommand(REG_TCTRL, 0b0110000000000111111000011111010);
    writeCommand(REG_TIPG, 0x0060200A);
}
void E1000::enableInterrupt()
{
    writeCommand(REG_IMASK, 0x1F6DC);
    writeCommand(REG_IMASK, 0xff & ~4);
    readCommand(0xc0);
}
void e1000_interrupt(CPURegisters* regs)
{
    (void)regs; // Unused parameter
    if ((0xC0) & 0x80)
    {
        while (defaultE1000->rxDescs[defaultE1000->rxIdx].status & 0x1)
        {
            recieveEthernetPacket((EthernetFrame*)defaultE1000->rxDescs[defaultE1000->rxIdx].addr,
                defaultE1000->rxDescs[defaultE1000->rxIdx].length, defaultE1000);
            defaultE1000->rxDescs[defaultE1000->rxIdx].status = 0;
            defaultE1000->writeCommand(REG_RXDESCTAIL, defaultE1000->rxIdx);
            defaultE1000->rxIdx = (defaultE1000->rxIdx + 1) % E1000_NUM_RX_DESC;
        }
    }
}
E1000::E1000(PCIDevice* dev)
{
    qemu_printf("0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
        dev->getBar(0), dev->getBar(1), dev->getBar(2), dev->getBar(3), dev->getBar(4));
    ioBase = dev->getBar(1) & 0xFFFC;
    mmioBase = dev->getBar(0) & 0xFFFFFFFC;
    kernelDirectory->mapPage(mmioBase, mmioBase, MMU_PRESENT | MMU_RW);
    dev->writeCommand(dev->getCommand() | (1 << 2));
    eepromExists = false;
    detectEEProm();
    uint32_t temp;
    temp = eepromRead(0);
    macAddress[0] = temp & 0xff;
    macAddress[1] = temp >> 8;
    temp = eepromRead(1);
    macAddress[2] = temp & 0xff;
    macAddress[3] = temp >> 8;
    temp = eepromRead(2);
    macAddress[4] = temp & 0xff;
    macAddress[5] = temp >> 8;
    qemu_printf("%x:%x:%x:%x:%x:%x\n", macAddress[0], macAddress[1], macAddress[2], macAddress[3],
        macAddress[4], macAddress[5]);
    for (size_t i = 0; i < 0x80; i++)
        writeCommand(0x5400 + i * 4, 0);
    registerIRQHandler(dev->getInterrupt(), e1000_interrupt);
    enableInterrupt();
    rxInit();
    txInit();
    defaultE1000 = this;
}
void E1000::sendPacket(Package* packet)
{
    txDescs[txIdx].addr = (uint64_t)packet->data;
    txDescs[txIdx].length = packet->len;
    txDescs[txIdx].cmd = CMD_EOP | CMD_IFCS | CMD_RS;
    txDescs[txIdx].status = 0;
    txIdx = (txIdx + 1) % E1000_NUM_TX_DESC;
    writeCommand(REG_TXDESCTAIL, txIdx);   
    while(!(txDescs[txIdx].status & 0xff));
}
Package* E1000::recievePacket()
{
    return NULL;
}
uint8_t* E1000::getMAC()
{
    return macAddress;
}