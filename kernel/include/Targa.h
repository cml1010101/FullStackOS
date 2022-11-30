#ifndef TARGA_H
#define TARGA_H
#include <SmartOS.h>
#include <Icon.h>
#ifdef __cplusplus
struct __attribute__((packed)) TargaHeader
{
    uint8_t idLength;
    uint8_t colorMapType;
    uint8_t imageType;
    uint8_t colorMapSpec[5];
    uint16_t xOrigin;
    uint16_t yOrigin;
    uint16_t imageWidth;
    uint16_t imageHeight;
    uint8_t pixelDepth;
    uint8_t alphaDepth: 4;
    uint8_t pixelOrdering: 4;
};
Icon* parseTGA(uint8_t* tgaPointer, size_t targaSize);
#endif
#endif