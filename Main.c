#include <stddef.h>
#include "gnu-efi/inc/efi.h"

void PutPixel(EFI_GRAPHICS_OUTPUT_PROTOCOL* gop, size_t x, size_t y, uint32_t color) {
    *((uint32_t*)(gop->Mode->FrameBufferBase + 4 * gop->Mode->Info->PixelsPerScanLine * y + 4 * x)) = color;
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

    for (size_t y = 0; y < gop->Mode->Info->VerticalResolution; y++) {
        for (size_t x = 0; x < gop->Mode->Info->HorizontalResolution; x++) {
            PutPixel(gop, x, y, 0x00FF0000);
        }
    }

    while (1)
        ;
    return EFI_SUCCESS;
}
