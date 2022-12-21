#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <SmartOS.h>
#ifdef __cplusplus
struct KeyboardState
{
    uint8_t leftShift: 1;
};
#define KEY_SHIFT 0x2A
#define KEY_ENTER 0x1C
extern KeyboardState keyboardState;
const char keycodesShiftASCII[0x76] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0, 'Q', 'W', 'E', 'R', 
    'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0xa, 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
    ':', '\"', '~', 0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, 0, 0, ' '
};
const char keycodesASCII[0x76] = {
    0, 0, '1', '2', '3', '4', '5', '6',     
    '7', '8', '9', '0', '-', '=', 0, 0, 'q', 'w', 'e', 'r', 't', 'y', 
    'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's', 'd', 'f', 'g', 
    'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 
    'b', 'n', 'm', ',', '.', '/', 0, 0, 0, ' '
};
typedef void(*KeyboardHandler)(uint8_t key, uint8_t pressed);
void initializeKeyboard();
void addKeyboardHandler(KeyboardHandler handler);
#endif
#endif