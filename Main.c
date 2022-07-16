#include <stddef.h>
#include <stdbool.h>
#include "gnu-efi/inc/efi.h"

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

typedef struct {
    char* Data;
    size_t Length;
} String;

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

void WriteString(EFI_SYSTEM_TABLE* SystemTable, String string) {
    WCHAR buffer[string.Length + 1];
    for (size_t i = 0; i < string.Length; i++) {
        buffer[i] = (WCHAR)string.Data[i];
    }
    buffer[string.Length] = 0;
    SystemTable->ConOut->OutputString(SystemTable->ConOut, buffer);
}

String UInt64ToString(char buffer[static 20], uint64_t value) {
    size_t length = 0;
    {
        uint64_t temp = value;
        while (temp != 0) {
            temp /= 10;
            length++;
        }
    }
    if (length > 0) {
        for (size_t i = 0; i < length; i++) {
            buffer[length - i - 1] = value % 10 + '0';
            value /= 10;
        }
    } else {
        buffer[0] = '0';
        length++;
    }
    return (String){ .Data = buffer, .Length = length };
}

EFI_STATUS EFIAPI EfiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
    EFI_STATUS status = SystemTable->BootServices->LocateProtocol(&gopGuid, NULL, (void**)&gop);
    if (EFI_ERROR(status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Unable to locate GOP\r\n");
        return status;
    }
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Located GOP\r\n");

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info;
    UINTN SizeOfInfo, numModes, nativeMode;
    status = gop->QueryMode(gop, gop->Mode == NULL ? 0 : gop->Mode->Mode, &SizeOfInfo, &info);
    if (status == EFI_NOT_STARTED)
        status = gop->SetMode(gop, 0);
    if (EFI_ERROR(status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Unable to get native GOP mode\r\n");
        return status;
    } else {
        nativeMode = gop->Mode->Mode;
        numModes   = gop->Mode->MaxMode;
    }
    (void)nativeMode;
    (void)numModes;

    // for (size_t i = 0; i < (size_t)numModes; i++) {
    //     status = gop->QueryMode(gop, i, &SizeOfInfo, &info);
    //     SystemTable->ConOut->OutputString(SystemTable->ConOut, L"mode ");
    //     char buffer[20];
    //     WriteString(SystemTable, UInt64ToString(buffer, i));
    //     SystemTable->ConOut->OutputString(SystemTable->ConOut, L" width ");
    //     WriteString(SystemTable, UInt64ToString(buffer, info->HorizontalResolution));
    //     SystemTable->ConOut->OutputString(SystemTable->ConOut, L" height ");
    //     WriteString(SystemTable, UInt64ToString(buffer, info->VerticalResolution));
    //     SystemTable->ConOut->OutputString(SystemTable->ConOut, L" format ");
    //     WriteString(SystemTable, UInt64ToString(buffer, info->PixelFormat));
    //     SystemTable->ConOut->OutputString(SystemTable->ConOut, i == nativeMode ? L" (current)\r\n" : L"\r\n");
    // }

    if (info->PixelFormat != PixelRedGreenBlueReserved8BitPerColor &&
        info->PixelFormat != PixelBlueGreenRedReserved8BitPerColor) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Unsupported GOP pixel format\r\n");
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
