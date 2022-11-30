#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <SmartOS.h>
#include <Icon.h>
#ifdef __cplusplus
class Font
{
public:
    uint8_t* glyphs;
    size_t fontSizeX, fontSizeY;
    size_t bytesPerGlyph;
    Font(void* src);
    uint8_t* getCharacterGlyph(unsigned char c);
};
class Window
{
public:
    uint32_t* framebuffer;
    size_t windowX, windowY;
    size_t width, height;
    Window(size_t x, size_t y, size_t w, size_t h);
    void fillRect(size_t x, size_t y, size_t w, size_t h, uint32_t color);
    void drawChar(size_t x, size_t y, unsigned char c, uint32_t color, Font* font);
};
void initializeGraphics(EFI_GRAPHICS_OUTPUT_PROTOCOL* gop);
Window* generateWindow(size_t x, size_t y, size_t w, size_t h);
void setCursor(Icon* cursorData);
void initializeCursor();
#endif
#endif