#include <PCI.h>
uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
    address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) 
        | ((uint32_t)0x80000000));
    outl(0xCF8, address);
    tmp = (uint16_t)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
    return tmp;
}
PCIDevice::PCIDevice(uint8_t bus, uint8_t slot, uint8_t func) : bus(bus), slot(slot), func(func)
{
}
uint16_t PCIDevice::getVendor()
{
    return pciConfigReadWord(bus, slot, func, 0);
}
uint16_t PCIDevice::getDeviceID()
{
    return pciConfigReadWord(bus, slot, func, 2);
}
uint8_t PCIDevice::getClass()
{
    return pciConfigReadWord(bus, slot, func, 0xA) >> 8;
}
uint8_t PCIDevice::getSubclass()
{
    return pciConfigReadWord(bus, slot, func, 0xA) & 0xFF;
}
uint8_t PCIDevice::getProgIF()
{
    return pciConfigReadWord(bus, slot, func, 0x8) >> 8;
}
uint8_t PCIDevice::getHeaderType()
{
    return pciConfigReadWord(bus, slot, func, 0xE) & 0xFF;
}
uint32_t PCIDevice::getBar(size_t bar)
{
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | ((0x10 + bar * 4) & 0xFC) 
        | ((uint32_t)0x80000000));
    outl(0xCF8, address);
    return inl(0xCFC);
}
uint8_t PCIDevice::getInterrupt()
{
    return pciConfigReadWord(bus, slot, func, 0x3C) & 0xFF;
}
uint16_t PCIDevice::getCommand()
{
    return pciConfigReadWord(bus, slot, func, 0x4);
}
void PCIDevice::writeCommand(uint16_t cmd)
{
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint32_t tmp = 0;
    address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | ((0x4) & 0xFC) 
        | ((uint32_t)0x80000000));
    outl(0xCF8, address);
    tmp = inl(0xCFC);
    outl(0xCF8, address);
    outl(0xCFC, cmd | tmp);
}
Vector<PCIDevice> pciDevices;
void initializePCI()
{
    pciDevices = Vector<PCIDevice>();
    for (size_t i = 0; i < 256; i++)
    {
        for (size_t j = 0; j < 16; j++)
        {
            PCIDevice dev = PCIDevice(i, j, 0);
            if (dev.getVendor() == 0xFFFF) continue;
            pciDevices.push(dev);
#ifdef __DEBUG__
            qemu_printf("PCI Device(%d, %d, 0): Type = {0x%x, 0x%x, 0x%x}, Vendor = 0x%x, Device = 0x%x\n",
                i, j, dev.getClass(), dev.getSubclass(), dev.getProgIF(), dev.getVendor(), dev.getDeviceID());
#endif
            if (dev.getHeaderType() & 0x80)
            {
                for (size_t k = 1; k < 8; k++)
                {
                    dev = PCIDevice(i, j, k);
                    if (dev.getVendor() == 0xFFFF) continue;
                    pciDevices.push(dev);
#ifdef __DEBUG__
                    qemu_printf("PCI Device(%d, %d, %d): Type = {0x%x, 0x%x, 0x%x}, Vendor = 0x%x, Device = 0x%x\n",
                        i, j, k, dev.getClass(), dev.getSubclass(), dev.getProgIF(), dev.getVendor(), dev.getDeviceID());
#endif
                }
            }
        }
    }
}