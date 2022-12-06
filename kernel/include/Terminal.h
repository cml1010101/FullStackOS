#ifndef TERMINAL_H
#define TERMINAL_H
#include <SmartOS.h>
#include <Graphics.h>
#ifdef __cplusplus
void clearTerminal();
void initializeTerminal(uint32_t fg, uint32_t bg, Font* font);
void putc(char c);
void print(const char* str);
#endif
#endif