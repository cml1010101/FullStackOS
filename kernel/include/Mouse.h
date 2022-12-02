#ifndef MOUSE_H
#define MOUSE_H
#include <SmartOS.h>
#ifdef __cplusplus
typedef void(*MouseClickHandler)(int x, int y);
typedef void(*MouseDragHandler)(int sourceX, int sourceY, int destX, int destY);
void addMouseClickHandler(MouseClickHandler handler);
void addMouseDragHandler(MouseDragHandler handler);
void initializeMouse();
extern int mouseX, mouseY;
#endif
#endif