#include <Terminal.h>
#include <Graphics.h>
uint32_t fg, bg;
int row, col, MAX_COLS, MAX_ROWS;
Font* font;
void clearTerminal()
{
    fillRect(0, 0, 1920, 1080, bg);
}
void initializeTerminal(uint32_t f, uint32_t b, Font* fo)
{
    fg = f;
    bg = b;
    font = fo;
    clearTerminal();
    row = 0;
    col = 0;
    MAX_COLS = 1920 / font->fontSizeX;
    MAX_ROWS = 1080 / font->fontSizeY;
}
void putc(char c)
{
    drawChar(col * font->fontSizeX, row * font->fontSizeY, c, fg, font);
    col++;
    if (col >= MAX_COLS)
    {
        col = 0;
        row++;
    }
}
void print(const char* str)
{
    for (size_t i = 0; i < strlen(str); i++)
    {
        putc(str[i]);
    }
}