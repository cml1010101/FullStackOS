#include <Graphics.h>
#include <MMU.h>
#include <Scheduler.h>
#define PSF_FONT_MAGIC 0x864ab572
struct PSFHeader {
    uint32_t magic;
    uint32_t version;
    uint32_t headersize;
    uint32_t flags;
    uint32_t numglyph;
    uint32_t bytesperglyph;
    uint32_t height;
    uint32_t width;
};
Font::Font(void* ptr)
{
    PSFHeader* psfHeader = (PSFHeader*)ptr;
    if (psfHeader->magic == PSF_FONT_MAGIC)
    {
        bytesPerGlyph = psfHeader->bytesperglyph;
        fontSizeX = psfHeader->width;
        fontSizeY = psfHeader->height;
        glyphs = new uint8_t[bytesPerGlyph * 256];
        memcpy(glyphs, psfHeader + 1, 256 * bytesPerGlyph);
    }
}
uint8_t* Font::getCharacterGlyph(unsigned char c)
{
    size_t idx = (size_t)c * bytesPerGlyph;
    return &glyphs[idx];
}
extern "C" char _binary_font_psf_start;
extern "C" char _binary_font_psf_end;
extern PageDirectory* kernelDirectory;
uint32_t* graphicsFramebuffer, *graphicsSecondary;
size_t graphicsWidth, graphicsHeight;
void fillRect(size_t x, size_t y, size_t w, size_t h, uint32_t color)
{
    size_t where = y * graphicsWidth + x;
    for (size_t i = 0; i < h; i++)
    {
        for (size_t j = 0; j < w; j++)
        {
            graphicsSecondary[where + j] = color;
        }
        where += graphicsWidth;
    }
}
void drawChar(size_t x, size_t y, unsigned char c, uint32_t color, Font* font)
{
    size_t where = y * graphicsWidth + x;
    uint8_t* glyph = font->getCharacterGlyph(c);
    for (size_t i = 0; i < font->fontSizeY; i++)
    {
        for (int j = font->fontSizeX - 1; j >= 0; j--)
        {
            if (glyph[i] & (1 << j)) graphicsSecondary[where + (font->fontSizeX - j - 1)] = color;
        }
        where += graphicsWidth;
    }
}
void graphicsUpdate()
{
    while (true)
    {
        size_t where = 0;
        for (size_t i = 0; i < graphicsHeight; i++)
        {
            for (size_t j = 0; j < graphicsWidth; j++)
            {
                graphicsFramebuffer[where + j] = graphicsSecondary[where + j];
            }
            where += graphicsWidth;
        }
    }
}
void initializeGraphics(EFI_GRAPHICS_OUTPUT_PROTOCOL* gop)
{
    graphicsFramebuffer = (uint32_t*)gop->Mode->FrameBufferBase;
    graphicsWidth = gop->Mode->Info->HorizontalResolution;
    graphicsHeight = gop->Mode->Info->VerticalResolution;
    graphicsSecondary = (uint32_t*)kmalloc(graphicsWidth * graphicsHeight * 4);
    kernelDirectory->map((uint64_t)graphicsFramebuffer, (uint64_t)graphicsFramebuffer,
        (graphicsWidth * graphicsHeight * 4 + 0xFFF) / 0x1000, MMU_PRESENT | MMU_RW);
    Thread* graphics = new Thread(graphicsUpdate, "GRAPHICS");
    addThread(graphics);
}