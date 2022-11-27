#ifndef FAT32_H
#define FAT32_H
#include <FAT.h>
#ifdef __cplusplus
struct __attribute__((packed)) FAT32BootBlock
{
    BPB bpb;
    uint32_t sectorsPerFAT;
    uint16_t flags;
    uint32_t rootCluster;
    uint16_t fsInfoSector;
    uint16_t backupSector;
    uint8_t reserved[12];
    uint8_t driveNumber;
    uint8_t windowsFlags;
    uint8_t signature;
    uint32_t serialNumber;
    char volumeLabel[11];
    char systemIdentifier[8];
    uint8_t bootcode[420];
    uint16_t bootSignature;
};
struct __attribute__((packed)) FSInfo
{
    uint32_t leadSignature;
    uint8_t reserved[480];
    uint32_t secondSignature;
    uint32_t lastKnownFreeCluster;
    uint8_t reserved[12];
    uint32_t trailSignature;
};
class FAT32 : public FileSystem
{
private:
    StorageDevice* device;
public:
    FAT32(StorageDevice* dev);
};
#endif
#endif