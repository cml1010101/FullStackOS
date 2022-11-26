#include <IDE.h>
#define ATA_REG_DATA 0x00
#define ATA_REG_ERROR 0x01
#define ATA_REG_FEATURES 0x01
#define ATA_REG_SECCOUNT0 0x02
#define ATA_REG_LBA0 0x03
#define ATA_REG_LBA1 0x04
#define ATA_REG_LBA2 0x05
#define ATA_REG_HDDEVSEL 0x06
#define ATA_REG_COMMAND 0x07
#define ATA_REG_STATUS 0x07
#define ATA_REG_SECCOUNT1 0x08
#define ATA_REG_LBA3 0x09
#define ATA_REG_LBA4 0x0A
#define ATA_REG_LBA5 0x0B
#define ATA_REG_CONTROL 0x0C
#define ATA_REG_ALTSTATUS 0x0C
#define ATA_REG_DEVADDRESS 0x0D
#define ATA_CMD_READ_PIO 0x20
#define ATA_CMD_READ_PIO_EXT 0x24
#define ATA_CMD_READ_DMA 0xC8
#define ATA_CMD_READ_DMA_EXT 0x25
#define ATA_CMD_WRITE_PIO 0x30
#define ATA_CMD_WRITE_PIO_EXT 0x34
#define ATA_CMD_WRITE_DMA 0xCA
#define ATA_CMD_WRITE_DMA_EXT 0x35
#define ATA_CMD_CACHE_FLUSH 0xE7
#define ATA_CMD_CACHE_FLUSH_EXT 0xEA
#define ATA_CMD_PACKET 0xA0
#define ATA_CMD_IDENTIFY_PACKET 0xA1
#define ATA_CMD_IDENTIFY 0xEC
#define ATA_IDENT_DEVICE_TYPE 0
#define ATA_IDENT_CYLINDERS 2
#define ATA_IDENT_HEADS 6
#define ATA_IDENT_SECTORS 12
#define ATA_IDENT_SERIAL 20
#define ATA_IDENT_MODEL 54
#define ATA_IDENT_CAPIBILITIES 98
#define ATA_IDENT_FIELDVALID 106
#define ATA_IDENT_MAX_LBA 120
#define ATA_IDENT_COMMANDSETS 164
#define ATA_IDENT_MAX_LBA_EXT 200
#define ATAPI_CMD_READ 0xA8
#define ATAPI_CMD_EJECT 0x1B
#define ATA_SR_BSY 0x80
#define ATA_SR_DRDY 0x40
#define ATA_SR_DF 0x20
#define ATA_SR_DSC 0x10
#define ATA_SR_DRQ 0x08
#define ATA_SR_CORR 0x04
#define ATA_SR_IDX 0x02
#define ATA_SR_ERR 0x01
uint8_t ideIRQInvoked;
IDEChannel::IDEChannel(uint16_t base, uint16_t control, uint16_t busMaster)
    : base(base), control(control), busMaster(busMaster)
{
    nInterrupt = 0x2;
}
uint8_t IDEChannel::read(uint8_t reg)
{
    uint8_t result;
    if (reg > 7 && reg < 0xC) write(ATA_REG_CONTROL, 0x80 | nInterrupt);
    if (reg < 8) result = inb(base + reg);
    else if (reg < 0xC) result = inb(base + reg - 6);
    else if (reg < 0xE) result = inb(control + reg - 0xA);
    else if (reg < 0x16) result = inb(busMaster + reg - 0xE);
    if (reg > 7 && reg < 0xC) write(ATA_REG_CONTROL, nInterrupt);
    return result;
}
void IDEChannel::write(uint8_t reg, uint8_t data)
{
    if (reg > 7 && reg < 0xC) write(ATA_REG_CONTROL, 0x80 | nInterrupt);
    if (reg < 8) outb(base + reg, data);
    else if (reg < 0xC) outb(base + reg - 0x6, data);
    else if (reg < 0xE) outb(control + reg - 0xA, data);
    else if (reg < 0x16) outb(busMaster + reg - 0xE, data);
    if (reg > 7 && reg < 0xC) write(ATA_REG_CONTROL, nInterrupt);
}
void IDEChannel::readBuffer(uint8_t reg, void* buffer, size_t quads)
{
    if (reg > 7 && reg < 0xC) write(ATA_REG_CONTROL, 0x80 | nInterrupt);
    if (reg < 8) 
    {
        asm volatile ("rep insl":: "d"(base + reg), "D"((uint64_t)buffer), "c"(quads));
    }
    else if (reg < 0xC) 
    {
        asm volatile ("rep insl":: "d"(base + reg - 0x6), "D"((uint64_t)buffer), "c"(quads));
    }
    else if (reg < 0xE) 
    {
        asm volatile ("rep insl":: "d"(control + reg - 0xA), "D"((uint64_t)buffer), "c"(quads));
    }
    else if (reg < 0x16) 
    {
        asm volatile ("rep insl":: "d"(busMaster + reg - 0xE), "D"((uint64_t)buffer), "c"(quads));
    }
    if (reg > 7 && reg < 0xC) write(ATA_REG_CONTROL, nInterrupt);
}
uint8_t IDEChannel::poll(bool advanced)
{
    for (size_t i = 0; i < 4; i++) read(ATA_REG_ALTSTATUS);
    while (read(ATA_REG_STATUS) & ATA_SR_BSY);
    if (advanced)
    {
        uint8_t state = read(ATA_REG_STATUS);
        if (state & ATA_SR_ERR) return 2;
        if (state & ATA_SR_DF) return 1;
        if ((state & ATA_SR_DRQ) == 0) return 3;
    }
    return 0;
}
IDEDevice::IDEDevice(IDEChannel* channel, uint8_t drive)
    : channel(channel), drive(drive)
{
    present = false;
    channel->write(ATA_REG_HDDEVSEL, 0xA0 | (drive << 4));
    sleep(1);
    channel->write(ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
    sleep(1);
    if (channel->read(ATA_REG_STATUS) == 0) return;
    uint8_t err = 0;
    type = IDE_ATA;
    while (true)
    {
        uint8_t status = channel->read(ATA_REG_STATUS);
        if (status & ATA_SR_ERR)
        {
            err = 1;
            break;
        }
        if (!(status & ATA_SR_BSY)) break;
    }
    if (err)
    {
        unsigned char cl = channel->read(ATA_REG_LBA1);
        unsigned char ch = channel->read(ATA_REG_LBA2);
        if (cl == 0x14 && ch == 0xEB) type = IDE_ATAPI;
        else if (cl == 0x69 && ch == 0x96) type = IDE_ATAPI;
        else return;
        channel->write(ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
        sleep(1);
    }
    present = true;
    uint8_t iden[512];
    channel->readBuffer(ATA_REG_DATA, iden, 128);
    signature = *(uint16_t*)&iden[ATA_IDENT_DEVICE_TYPE];
    capabilities = *(uint16_t*)&iden[ATA_IDENT_CAPIBILITIES];
    commandSets = *(uint32_t*)&iden[ATA_IDENT_COMMANDSETS];
    for (size_t i = 0; i < 40; i += 2)
    {
        deviceModel[i] = iden[ATA_IDENT_MODEL + i + 1];
        deviceModel[i + 1] = iden[ATA_IDENT_MODEL + i];
    }
    deviceModel[40] = 0;
    for (int i = 39; i >= 0; i--)
    {
        if (deviceModel[i + 1] == 0 && deviceModel[i] == ' ') deviceModel[i] = 0;
    }
    if (commandSets & (1 << 26)) sectors = *(uint64_t*)&iden[ATA_IDENT_MAX_LBA_EXT];
    else sectors = *(uint32_t*)&iden[ATA_IDENT_MAX_LBA];
}
const char* IDEDevice::getName()
{
    return deviceModel;
}
const char* IDEDevice::getType()
{
    return "Serial IDE Device";
}
bool IDEDevice::exists()
{
    return present;
}
IDEDriver::IDEDriver(PCIDevice dev)
{
    ideIRQInvoked = 0;
    pciDevice = dev;
    uint32_t bar0 = dev.getBar(0);
    if (!bar0) bar0 = 0x1F0;
    uint32_t bar1 = dev.getBar(1);
    if (!bar1) bar1 = 0x3F6;
    uint32_t bar2 = dev.getBar(2);
    if (!bar2) bar2 = 0x170;
    uint32_t bar3 = dev.getBar(3);
    if (!bar3) bar3 = 0x376;
    uint32_t bar4 = dev.getBar(4);
    qemu_printf("(0x%x, 0x%x, 0x%x, 0x%x, 0x%x)\n", bar0, bar1, bar2, bar3, bar4);
    channels[0] = IDEChannel(bar0, bar1, bar4);
    channels[1] = IDEChannel(bar2, bar3, bar4 + 8);
    channels[0].write(ATA_REG_CONTROL, 2);
    channels[1].write(ATA_REG_CONTROL, 2);
    devices = Vector<IDEDevice*>();
    for (size_t i = 0; i < 2; i++)
    {
        for (size_t j = 0; j < 2; j++)
        {
#ifdef __DEBUG__
            qemu_printf("Checking (%d, %d)\n", i, j);
#endif
            IDEDevice* device = new IDEDevice(&channels[i], j);
            if (device->exists())
            {
                devices.push(device);
                registerStorageDevice(device);
#ifdef __DEBUG__
                qemu_printf("Found IDE Device: %s with %d sectors.\n", device->deviceModel,
                    device->sectors);
#endif
            }
        }
    }
}
const char* IDEDriver::getName()
{
    return "IDE Driver";
}
void waitIDEIRQ()
{
    while (!ideIRQInvoked);
    ideIRQInvoked = 0;
}
void ideIRQ()
{
    ideIRQInvoked = 1;
}
void IDEDevice::readSectors(uint64_t lba, void* dest, size_t numSectors)
{
    if (numSectors > 256)
    {
        for (size_t i = 0; i < (numSectors & ~0xFF); i += 0x100)
        {
            readSectors(lba + i, dest + i * 512, 0x100);
        }
        if (numSectors & 0xFF)
        {
            readSectors(lba + (numSectors & ~0xFF), dest + (numSectors & ~0xFF) * 512,
                numSectors & 0xFF);
        }
    }
    else
    {
        uint8_t sectors = numSectors & 0xFF;
        if (type == IDE_ATA)
        {
            uint8_t mode;
            uint8_t io[6];
            uint8_t head, cyl, sect;
            channel->write(ATA_REG_CONTROL, channel->nInterrupt = (ideIRQInvoked = 0x0) + 0x02);
            if (lba >= 0x10000000)
            {
                mode = 2;
                io[0] = (lba & 0x0000000000FF) >> 0;
                io[1] = (lba & 0x00000000FF00) >> 8;
                io[2] = (lba & 0x000000FF0000) >> 16;
                io[3] = (lba & 0x0000FF000000) >> 24;
                io[4] = (lba & 0x00FF00000000) >> 32;
                io[5] = (lba & 0xFF0000000000) >> 40;
                head = 0;
            }
            else if (capabilities & 0x200)
            {
                mode = 1;
                io[0] = (lba & 0x00000FF) >> 0;
                io[1] = (lba & 0x000FF00) >> 8;
                io[2] = (lba & 0x0FF0000) >> 16;
                io[3] = 0;
                io[4] = 0;
                io[5] = 0;
                head = (lba & 0xF000000) >> 24;
            }
            else
            {
                mode = 0;
                sect = (lba % 63) + 1;
                cyl = (lba + 1 - sect) / (16 * 63);
                io[0] = sect;
                io[1] = (cyl >> 0) & 0xFF;
                io[2] = (cyl >> 8) & 0xFF;
                io[3] = 0;
                io[4] = 0;
                io[5] = 0;
                head = (lba + 1  - sect) % (16 * 63) / (63);
            }
            while (channel->read(ATA_REG_STATUS) & ATA_SR_BSY);
            if (mode == 0) channel->write(ATA_REG_HDDEVSEL, 0xA0 | (drive << 4) | head);
            else channel->write(ATA_REG_HDDEVSEL, 0xE0 | (drive << 4) | head);
            if (mode == 2)
            {
                channel->write(ATA_REG_SECCOUNT1, 0);
                channel->write(ATA_REG_LBA3, io[3]);
                channel->write(ATA_REG_LBA4, io[4]);
                channel->write(ATA_REG_LBA5, io[5]);
            }
            channel->write(ATA_REG_SECCOUNT0, sectors);
            channel->write(ATA_REG_LBA0, io[0]);
            channel->write(ATA_REG_LBA1, io[1]);
            channel->write(ATA_REG_LBA2, io[2]);
            uint8_t cmd = ATA_CMD_READ_PIO;
            if (mode == 2) cmd = ATA_CMD_READ_PIO_EXT;
            channel->write(ATA_REG_COMMAND, cmd);
            for (size_t i = 0; i < sectors; i++)
            {
                channel->poll(true);
                asm volatile ("rep insw":: "c"(256), "d"(channel->base), "D"(dest + i * 512));
            }
            cmd = ATA_CMD_CACHE_FLUSH;
            if (mode == 2) cmd = ATA_CMD_CACHE_FLUSH_EXT;
            channel->write(ATA_REG_COMMAND, cmd);
            channel->poll(false);
        }
        else
        {
            channel->write(ATA_REG_CONTROL, channel->nInterrupt = ideIRQInvoked = 0x0);
            uint8_t packet[12];
            packet[0] = ATAPI_CMD_READ;
            packet[1] = 0;
            packet[2] = (lba >> 24) & 0xFF;
            packet[3] = (lba >> 16) & 0xFF;
            packet[4] = (lba >> 8) & 0xFF;
            packet[5] = lba & 0xFF;
            packet[6] = 0;
            packet[7] = 0;
            packet[8] = 0;
            packet[9] = sectors;
            packet[10] = 0;
            packet[11] = 0;
            channel->write(ATA_REG_HDDEVSEL, drive << 4);
            for (size_t i = 0; i < 4; i++) channel->read(ATA_REG_ALTSTATUS);
            channel->write(ATA_REG_FEATURES, 0);
            channel->write(ATA_REG_LBA1, 2048 & 0xFF);
            channel->write(ATA_REG_LBA2, (2048 >> 8) & 0xFF);
            channel->write(ATA_REG_COMMAND, ATA_CMD_PACKET);
            asm volatile ("rep outsw":: "c"(6), "d"(channel->base), "S"(packet));
            for (size_t i = 0; i < sectors; i++)
            {
                waitIDEIRQ();
                asm volatile ("rep insw":: "c"(1024), "d"(channel->base), "D"(dest + i * 2048));
            }
            waitIDEIRQ();
            while (channel->read(ATA_REG_STATUS) & (ATA_SR_BSY | ATA_SR_DRQ));
        }
    }
}
void IDEDevice::writeSectors(uint64_t lba, const void* src, size_t numSectors)
{
    if (numSectors > 256)
    {
        for (size_t i = 0; i < (numSectors & ~0xFF); i += 0x100)
        {
            writeSectors(lba + i, src + i * 512, 0x100);
        }
        if (numSectors & 0xFF)
        {
            writeSectors(lba + (numSectors & ~0xFF), src + (numSectors & ~0xFF) * 512,
                numSectors & 0xFF);
        }
    }
    else
    {
        if (type == IDE_ATA)
        {
            uint8_t sectors = numSectors & 0xFF;
            uint8_t mode;
            uint8_t io[6];
            uint8_t head, cyl, sect;
            channel->write(ATA_REG_CONTROL, channel->nInterrupt = (ideIRQInvoked = 0x0) + 0x02);
            if (lba >= 0x10000000)
            {
                mode = 2;
                io[0] = (lba & 0x0000000000FF) >> 0;
                io[1] = (lba & 0x00000000FF00) >> 8;
                io[2] = (lba & 0x000000FF0000) >> 16;
                io[3] = (lba & 0x0000FF000000) >> 24;
                io[4] = (lba & 0x00FF00000000) >> 32;
                io[5] = (lba & 0xFF0000000000) >> 40;
                head = 0;
            }
            else if (capabilities & 0x200)
            {
                mode = 1;
                io[0] = (lba & 0x00000FF) >> 0;
                io[1] = (lba & 0x000FF00) >> 8;
                io[2] = (lba & 0x0FF0000) >> 16;
                io[3] = 0;
                io[4] = 0;
                io[5] = 0;
                head = (lba & 0xF000000) >> 24;
            }
            else
            {
                mode = 0;
                sect = (lba % 63) + 1;
                cyl = (lba + 1 - sect) / (16 * 63);
                io[0] = sect;
                io[1] = (cyl >> 0) & 0xFF;
                io[2] = (cyl >> 8) & 0xFF;
                io[3] = 0;
                io[4] = 0;
                io[5] = 0;
                head = (lba + 1  - sect) % (16 * 63) / (63);
            }
            while (channel->read(ATA_REG_STATUS) & ATA_SR_BSY);
            if (mode == 0) channel->write(ATA_REG_HDDEVSEL, 0xA0 | (drive << 4) | head);
            else channel->write(ATA_REG_HDDEVSEL, 0xE0 | (drive << 4) | head);
            if (mode == 2)
            {
                channel->write(ATA_REG_SECCOUNT1, 0);
                channel->write(ATA_REG_LBA3, io[3]);
                channel->write(ATA_REG_LBA4, io[4]);
                channel->write(ATA_REG_LBA5, io[5]);
            }
            channel->write(ATA_REG_SECCOUNT0, sectors);
            channel->write(ATA_REG_LBA0, io[0]);
            channel->write(ATA_REG_LBA1, io[1]);
            channel->write(ATA_REG_LBA2, io[2]);
            uint8_t cmd = ATA_CMD_WRITE_PIO;
            if (mode == 2) cmd = ATA_CMD_WRITE_PIO_EXT;
            channel->write(ATA_REG_COMMAND, cmd);
            for (size_t i = 0; i < sectors; i++)
            {
                channel->poll(true);
                asm volatile ("rep outsw":: "c"(256), "d"(channel->base), "S"(src + i * 512));
            }
            cmd = ATA_CMD_CACHE_FLUSH;
            if (mode == 2) cmd = ATA_CMD_CACHE_FLUSH_EXT;
            channel->write(ATA_REG_COMMAND, cmd);
            channel->poll(false);
        }
    }
}