#ifndef IDE_H
#define IDE_H
#include <SmartOS.h>
#include <PCI.h>
#ifdef __cplusplus
class IDEDriver;
class IDEChannel
{
public:
    uint16_t base, control, busMaster;
    uint8_t nInterrupt;
    IDEChannel() = default;
    IDEChannel(uint16_t base, uint16_t control, uint16_t busMaster);
    uint8_t read(uint8_t reg);
    void write(uint8_t reg, uint8_t data);
    void readBuffer(uint8_t reg, void* buffer, size_t quads);
    uint8_t poll(bool advanced);
};
class IDEDevice : public StorageDevice
{
private:
    friend class IDEDriver;
    IDEChannel* channel;
    uint8_t drive;
    bool present;
    uint64_t type, sectors;
    uint16_t signature, capabilities;
    uint32_t commandSets;
    char deviceModel[41];
    enum DeviceType
    {
        IDE_ATA,
        IDE_ATAPI
    };
public:
    IDEDevice() = default;
    IDEDevice(IDEChannel* channel, uint8_t drive);
    const char* getName();
    const char* getType();
    bool exists();
    void readSectors(uint64_t lba, void* dest, size_t sectors);
    void writeSectors(uint64_t lba, const void* src, size_t sectors);
    size_t getSize();
};
class IDEDriver : public Driver
{
private:
    PCIDevice pciDevice;
    IDEChannel channels[2];
    friend class IDEDevice;
public:
    Vector<IDEDevice*> devices;
    IDEDriver() = default;
    IDEDriver(PCIDevice dev);
    const char* getName();
};
#endif
#endif