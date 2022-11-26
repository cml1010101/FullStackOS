#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <SmartOS.h>
#ifdef __cplusplus
class Window
{
public:
    uint32_t* framebuffer;
    size_t windowX, windowY;
    size_t width, height;
    Window(size_t x, size_t y, size_t w, size_t h);
    void fillRect(size_t x, size_t y, size_t w, size_t h, uint32_t color);
};
void initializeGraphics(EFI_GRAPHICS_OUTPUT_PROTOCOL* gop);
Window* generateWindow(size_t x, size_t y, size_t w, size_t h);
#endif
#endif