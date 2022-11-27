#include <Partitions.h>
#include <FAT16.h>
void scanDevice(StorageDevice* device)
{
    MBRBlock mbr;
    device->readSectors(0, &mbr, 1);
    for (size_t i = 0; i < 4; i++)
    {
        if (mbr.entries[i].system_id == 0x6)
        {
            fileSystems.push(new FAT16(device, mbr.entries[i].start_lba));
        }
    }
}