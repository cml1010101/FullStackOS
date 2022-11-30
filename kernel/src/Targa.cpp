#include <Targa.h>
Icon* parseTGA(uint8_t* targaPtr, size_t targaSize)
{
    size_t w, h;
    TargaHeader* header = (TargaHeader*)targaPtr;
    w = header->imageWidth;
    h = header->imageHeight;
    uint32_t* data = new uint32_t[w * h];
    uint8_t* targaData = targaPtr + sizeof(TargaHeader);
    if (header->imageType == 3)
    {
        for (size_t i = 0; i < h; i++)
        {
            for (size_t j = 0; j < w; j++)
            {
                size_t k = (h - i - 1) * w + j;
                data[i * w + j] = targaData[k] | (targaData[k] << 8) | (targaData[k] << 16);
            }
        }
    }
    return new Icon(w, h, data);
}