#ifndef FAT_H
#define FAT_H
#include <SmartOS.h>
#ifdef __cplusplus
struct __attribute__((packed)) BPB
{
    uint8_t jmp[3];
    uint8_t oem[8];
    uint16_t bytesPerSector;
    uint8_t sectorsPerCluster;
    uint16_t reservedSectors;
    uint8_t numberOfFAT;
    uint16_t numberOfRootEntries;
    uint16_t smallSectorsCount;
    uint8_t mediaType;
    uint16_t smallSectorsPerFAT;
    uint16_t sectorsPerTrack;
    uint16_t numberOfHeads;
    uint32_t largeSectorCount;
};
struct __attribute__((packed)) DirectoryEntry
{
    char name[11];
    uint8_t attr;
    uint8_t reserved;
    uint8_t creationMillis;
    uint16_t hour: 5;
    uint16_t minutes: 6;
    uint16_t seconds: 5;
    uint16_t year: 7;
    uint16_t month: 4;
    uint16_t day: 5;
    uint16_t lastAccessedYear: 7;
    uint16_t lastAccessedMonth: 4;
    uint16_t lastAccessedDay: 5;
    uint16_t clusterHigh;
    uint16_t modHour: 5;
    uint16_t modMinutes: 6;
    uint16_t modSeconds: 5;
    uint16_t modYear: 7;
    uint16_t modMonth: 4;
    uint16_t modDay: 5;
    uint16_t clusterLow;
    uint32_t size;
};
#endif
#endif