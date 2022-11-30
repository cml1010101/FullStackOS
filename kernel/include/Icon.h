#ifndef ICON_H
#define ICON_H
#include <SmartOS.h>
#ifdef __cplusplus
class Icon
{
private:
    size_t width, height;
    uint32_t* data;
public:
    inline Icon(size_t width, size_t height, uint32_t* data)
    {
        this->width = width;
        this->height = height;
        this->data = data;
    }
    inline size_t getWidth()
    {
        return width;
    }
    inline size_t getHeight()
    {
        return height;
    }
    inline uint32_t* getData()
    {
        return data;
    }
};
#endif
#endif