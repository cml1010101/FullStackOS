#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <SmartOS.h>
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
void initializeGraphics(EFI_GRAPHICS_OUTPUT_PROTOCOL* gop);
void fillRect(size_t x, size_t y, size_t w, size_t h, uint32_t color);
void drawChar(size_t x, size_t y, unsigned char c, uint32_t color, Font* font);
#endif
#endif