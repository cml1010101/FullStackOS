#include <Graphics.h>
#include <CPU.h>
Window::Window(size_t x, size_t y, size_t w, size_t h)
{
    framebuffer = (uint32_t*)cpu.mmu.kmalloc(w * h * 4, false);
    windowX = x;
    windowY = y;
    width = w;
    height = h;
    sourceColor = 0;
}
void Window::setColor(uint32_t color)
{
    sourceColor = color;
}
void Window::drawLine(size_t x1, size_t y1, size_t x2, size_t y2)
{
    double dx = x2 - x1;
    double dy = y2 - y1;
    double startX = x1, startY = y1, endX = x2, endY = y2;
    size_t where = y1 * width + x1;
    if (abs(dx) >= abs(dy))
    {
        double xInc = 1;
        double yInc = dy / dx;
        for (size_t i = 0; i < abs(dx); i++)
        {
            framebuffer[where + (size_t)startX + (size_t)(startY * width)] = sourceColor;
            startX += xInc;
            startY += yInc;
        }
    }
    else
    {
        double xInc = dx / dy;
        double yInc = 1;
        for (size_t i = 0; i < abs(dy); i++)
        {
            framebuffer[where + (size_t)startX + (size_t)(startY * width)] = sourceColor;
            startX += xInc;
            startY += yInc;
        }
    }
}