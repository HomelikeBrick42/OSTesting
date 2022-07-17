#pragma once

#include <stddef.h>
#include "gnu-efi/inc/efi.h"

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

void PutPixel(EFI_GRAPHICS_OUTPUT_PROTOCOL* gop, EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info, size_t x, size_t y, Color color);

enum {
    CHAR_WIDTH  = 8,
    CHAR_HEIGHT = 13,
};
extern unsigned char Font[95][13];
void PutChar(
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop, EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info, size_t x, size_t y, char chr, Color color);

typedef struct {
    char* Data;
    size_t Length;
} String;
#define String_FromLiteral(s)               \
    (String) {                              \
        .Data = s, .Length = sizeof(s) - 1, \
    }

void PutString(EFI_GRAPHICS_OUTPUT_PROTOCOL* gop,
               EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info,
               size_t startX,
               size_t* x,
               size_t* y,
               String string,
               Color color);
