#include <stddef.h>
#include <stdbool.h>
#include "gnu-efi/inc/efi.h"

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

void PutPixel(EFI_GRAPHICS_OUTPUT_PROTOCOL* gop, EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info, size_t x, size_t y, Color color) {
    uint8_t* pixel = (uint8_t*)(gop->Mode->FrameBufferBase + 4 * gop->Mode->Info->PixelsPerScanLine * y + 4 * x);
    if (info->PixelFormat == PixelRedGreenBlueReserved8BitPerColor) {
        pixel[0] = color.r;
        pixel[1] = color.g;
        pixel[2] = color.b;
    } else if (info->PixelFormat == PixelBlueGreenRedReserved8BitPerColor) {
        pixel[0] = color.b;
        pixel[1] = color.g;
        pixel[2] = color.r;
    } else {
        // do nothing
    }
}

EFI_STATUS EFIAPI EfiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
    EFI_STATUS status = SystemTable->BootServices->LocateProtocol(&gopGuid, NULL, (void**)&gop);
    if (EFI_ERROR(status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Unable to locate GOP\n");
        return status;
    }
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Located GOP\n");

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info;
    UINTN SizeOfInfo, numModes, nativeMode;
    status = gop->QueryMode(gop, gop->Mode == NULL ? 0 : gop->Mode->Mode, &SizeOfInfo, &info);
    if (status == EFI_NOT_STARTED)
        status = gop->SetMode(gop, 0);
    if (EFI_ERROR(status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Unable to get native GOP mode\n");
        return status;
    } else {
        nativeMode = gop->Mode->Mode;
        numModes   = gop->Mode->MaxMode;
    }
    (void)nativeMode;
    (void)numModes;

    if (info->PixelFormat != PixelRedGreenBlueReserved8BitPerColor &&
        info->PixelFormat != PixelBlueGreenRedReserved8BitPerColor) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Unsupported GOP pixel format\n");
        return EFI_UNSUPPORTED;
    }

    for (size_t y = 0; y < gop->Mode->Info->VerticalResolution; y++) {
        for (size_t x = 0; x < gop->Mode->Info->HorizontalResolution; x++) {
            PutPixel(gop, info, x, y, (Color){ .r = 255, .g = 0, .b = 0 });
        }
    }

    while (true) {
        asm volatile("hlt");
    }

    return EFI_SUCCESS;
}
