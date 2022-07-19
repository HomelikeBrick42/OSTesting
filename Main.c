#include "IO.h"
#include "Kernel.h"

#include "gnu-efi/inc/efi.h"

#include <stddef.h>
#include <stdbool.h>

static void WriteString(EFI_SYSTEM_TABLE* SystemTable, String string) {
    WCHAR buffer[string.Length + 1];
    for (size_t i = 0; i < string.Length; i++) {
        buffer[i] = (WCHAR)string.Data[i];
    }
    buffer[string.Length] = 0;
    SystemTable->ConOut->OutputString(SystemTable->ConOut, buffer);
}

static EFI_STATUS LocateGOP(EFI_SYSTEM_TABLE* SystemTable, EFI_GRAPHICS_OUTPUT_PROTOCOL** gop) {
    EFI_GUID gopGuid  = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_STATUS status = SystemTable->BootServices->LocateProtocol(&gopGuid, NULL, (void**)gop);
    if (EFI_ERROR(status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Unable to locate GOP\r\n");
        return status;
    }
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Located GOP\r\n");

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info;
    UINTN sizeOfInfo, numModes, nativeMode;
    status = (*gop)->QueryMode((*gop), (*gop)->Mode == NULL ? 0 : (*gop)->Mode->Mode, &sizeOfInfo, &info);
    if (status == EFI_NOT_STARTED)
        status = (*gop)->SetMode((*gop), 0);
    if (EFI_ERROR(status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Unable to get native GOP mode\r\n");
        return status;
    } else {
        nativeMode = (*gop)->Mode->Mode;
        numModes   = (*gop)->Mode->MaxMode;
    }

    if (false) {
        for (size_t i = 0; i < (size_t)numModes; i++) {
            status = (*gop)->QueryMode((*gop), i, &sizeOfInfo, &info);
            SystemTable->ConOut->OutputString(SystemTable->ConOut, L"mode ");
            char buffer[20];
            WriteString(SystemTable, UInt64ToString(buffer, i));
            SystemTable->ConOut->OutputString(SystemTable->ConOut, L" width ");
            WriteString(SystemTable, UInt64ToString(buffer, info->HorizontalResolution));
            SystemTable->ConOut->OutputString(SystemTable->ConOut, L" height ");
            WriteString(SystemTable, UInt64ToString(buffer, info->VerticalResolution));
            SystemTable->ConOut->OutputString(SystemTable->ConOut, L" format ");
            WriteString(SystemTable, UInt64ToString(buffer, info->PixelFormat));
            SystemTable->ConOut->OutputString(SystemTable->ConOut, i == nativeMode ? L" (current)\r\n" : L"\r\n");
        }
    }

    if (info->PixelFormat != PixelRedGreenBlueReserved8BitPerColor &&
        info->PixelFormat != PixelBlueGreenRedReserved8BitPerColor) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Unsupported GOP pixel format\r\n");
        return EFI_UNSUPPORTED;
    }

    return EFI_SUCCESS;
}

EFI_STATUS EFIAPI EfiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
    EFI_STATUS status = LocateGOP(SystemTable, &gop);
    if (EFI_ERROR(status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Unable to load GOP\r\n");
        return status;
    }

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info;
    UINTN SizeOfInfo;
    status = gop->QueryMode(gop, gop->Mode == NULL ? 0 : gop->Mode->Mode, &SizeOfInfo, &info);
    if (EFI_ERROR(status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Unable to get GOP info\r\n");
        return status;
    }

    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Loaded GOP\r\n");

    EFI_MEMORY_DESCRIPTOR* map = NULL;
    UINTN mapSize = 0, mapKey = 0, descriptorSize = 0;
    UINT32 descriptorVersion = 0;
    while (true) {
        status = SystemTable->BootServices->GetMemoryMap(&mapSize, map, &mapKey, &descriptorSize, &descriptorVersion);
        if (status == EFI_BUFFER_TOO_SMALL) {
            if (map != NULL) {
                status = SystemTable->BootServices->FreePool(map);
                if (EFI_ERROR(status)) {
                    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Unable free memory for the memory map\r\n");
                    return status;
                }
            }
            status = SystemTable->BootServices->AllocatePool(EfiLoaderData, mapSize, (void**)&map);
            if (EFI_ERROR(status)) {
                SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Unable allocate memory for the memory map\r\n");
                return status;
            }
            continue;
        } else if (EFI_ERROR(status)) {
            SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Unable to get the memory map size\r\n");
            return status;
        } else {
            break;
        }
    }
    SystemTable->BootServices->ExitBootServices(ImageHandle, mapKey);

    Screen.Buffer            = (void*)gop->Mode->FrameBufferBase;
    Screen.Width             = gop->Mode->Info->HorizontalResolution;
    Screen.Height            = gop->Mode->Info->VerticalResolution;
    Screen.PixelsPerScanline = gop->Mode->Info->PixelsPerScanLine;
    Screen.Format            = info->PixelFormat == PixelRedGreenBlueReserved8BitPerColor   ? FramebufferFormat_ARGB
                               : info->PixelFormat == PixelBlueGreenRedReserved8BitPerColor ? FramebufferFormat_ABGR
                                                                                            : FramebufferFormat_Invalid;

    KernelMain((EfiMemoryDescriptor*)map, mapSize, descriptorSize);

    return EFI_SUCCESS;
}
