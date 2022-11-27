#ifndef MBR_H
#define MBR_H
#include <SmartOS.h>
#ifdef __cplusplus
struct __attribute__((packed)) MBREntry
{
    uint8_t indicator;
    uint8_t head;
    uint16_t sect: 6;
    uint16_t cyl: 10;
    uint8_t system_id;
    uint8_t end_head;
    uint16_t end_sect: 6;
    uint16_t end_cyl: 10;
    uint32_t start_lba;
    uint32_t lba_size;
};
struct __attribute__((packed)) MBRBlock
{
    uint8_t rsv[446];
    MBREntry entries[4];
    uint16_t magic;
};
void scanDevice(StorageDevice* device);
#endif
#endif