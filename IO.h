#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
    void* Buffer;
    size_t Width;
    size_t Height;
    size_t PixelsPerScanline;
    enum {
        FramebufferFormat_Invalid,
        FramebufferFormat_ARGB,
        FramebufferFormat_ABGR,
    } Format;
} Framebuffer;

extern Framebuffer Screen;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

void PutPixel(size_t x, size_t y, Color color);
void FillRect(size_t x, size_t y, size_t width, size_t height, Color color);

enum {
    CHAR_WIDTH  = 8,
    CHAR_HEIGHT = 13,
};
extern unsigned char Font[95][13];
void PutChar(size_t x, size_t y, char chr, Color backgroundColor, Color textColor);

typedef struct {
    char* Data;
    size_t Length;
} String;
#define String_FromLiteral(s)               \
    (String) {                              \
        .Data = s, .Length = sizeof(s) - 1, \
    }

void PutString(size_t startX, size_t* x, size_t* y, String string, Color backgroundColor, Color textColor);
