#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <SmartOS.h>
#ifdef __cplusplus
class Window
{
private:
    uint32_t* framebuffer;
    size_t width, height, windowX, windowY;
    uint32_t sourceColor;
    friend class Graphics;
public:
    Window() = default;
    Window(size_t x, size_t y, size_t w, size_t h);
    void setColor(uint32_t color);
    void drawLine(size_t x1, size_t y1, size_t x2, size_t y2);
    void drawRect(size_t x, size_t y, size_t w, size_t h);
    void drawRoundRect(size_t x, size_t y, size_t w, size_t h, size_t xRadius, size_t yRadius);
    void fillRect(size_t x, size_t y, size_t w, size_t h);
    void fillRoundRect(size_t x, size_t y, size_t w, size_t h, size_t xRadius, size_t yRadius);
    void drawChar(size_t x, size_t y, char c);
    void drawString(size_t x, size_t y, const char* str);
};
class Graphics : public Driver
{
private:
    uint32_t* framebuffer;
    size_t width, height;
    uint32_t sourceColor;
    Vector<Window*> windows;
public:
    Graphics() = default;
    Graphics(EFI_GRAPHICS_OUTPUT_PROTOCOL* gop);
    Window* generateWindow(size_t x, size_t y, size_t w, size_t h);
    const char* getName();
};
extern Graphics* graphics;
#endif
#endif