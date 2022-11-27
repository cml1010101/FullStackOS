#ifndef FAT16_H
#define FAT16_H
#include <FAT.h>
#ifdef __cplusplus
struct __attribute__((packed)) FAT16BootBlock
{
    BPB bpb;
    uint8_t driveNumber;
    uint8_t windowsFlags;
    uint8_t signature;
    uint32_t volumeSerial;
    char volumeLabel[11];
    char systemIdentifier[8];
    uint8_t bootcode[448];
    uint16_t bootSignature;
};
class FAT16 : public FileSystem
{
private:
    StorageDevice* device;
    size_t partitionOffset, sectorsPerCluster, sectorsPerFAT, fatOffset, dataOffset, rootOffset;
    uint16_t readFAT(uint16_t entry);
    void writeFAT(uint16_t idx, uint16_t entry);
    void readCluster(uint16_t idx, void* buffer);
    void writeCluster(uint16_t idx, const void* buffer);
    void getEntry(uint16_t idx, const char* fname);
    uint16_t searchDirectory(uint16_t cluster, const char* fatname, size_t* size);
    void read(File* metadata, void* dest, size_t size);
    void write(File* metadata, const void* src, size_t size);
public:
    File* open(const char* path);
    Vector<File*> list(const char* path);
    FAT16(StorageDevice* dev, size_t offset);
    const char* getName();
};
#endif
#endif