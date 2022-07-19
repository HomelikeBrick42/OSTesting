#include <stddef.h>
#include <stdbool.h>
#include "gnu-efi/inc/efi.h"
#include "IO.h"
#include "IDT.h"
#include "Interrupts.h"

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

EFI_STATUS LocateGOP(EFI_SYSTEM_TABLE* SystemTable, EFI_GRAPHICS_OUTPUT_PROTOCOL** gop) {
    EFI_GUID gopGuid  = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_STATUS status = SystemTable->BootServices->LocateProtocol(&gopGuid, NULL, (void**)gop);
    if (EFI_ERROR(status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Unable to locate GOP\r\n");
        return status;
    }
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Located GOP\r\n");

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info;
    UINTN SizeOfInfo, numModes, nativeMode;
    status = (*gop)->QueryMode((*gop), (*gop)->Mode == NULL ? 0 : (*gop)->Mode->Mode, &SizeOfInfo, &info);
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
            status = (*gop)->QueryMode((*gop), i, &SizeOfInfo, &info);
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

IDTR GlobalIDTR;
EFI_STATUS PrepareInterrupts(EFI_SYSTEM_TABLE* SystemTable) {
    asm volatile("cli");

    GlobalIDTR.Limit = 0xFFFF;
    EFI_PHYSICAL_ADDRESS ptr;
    EFI_STATUS status = SystemTable->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData, 1, &ptr);
    if (EFI_ERROR(status) || ptr == 0) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Unable to allocate page for interrupts\r\n");
        return status;
    }
    GlobalIDTR.Offset = (uint64_t)ptr;

    // IDTDescriptorEntry* pageFault = (IDTDescriptorEntry*)(GlobalIDTR.Offset + 0xE * sizeof(IDTDescriptorEntry));
    // IDTDescriptorEntry_SetOffset(pageFault, (uint64_t)PageFault_Handler);
    // pageFault->TypesAttributes = IDT_TA_InterruptGate;
    // pageFault->Selector        = 0x08;

    asm volatile("lidt %0" : : "m"(GlobalIDTR));

    asm volatile("sti");
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

    Screen.Buffer            = (void*)gop->Mode->FrameBufferBase;
    Screen.Width             = gop->Mode->Info->HorizontalResolution;
    Screen.Height            = gop->Mode->Info->VerticalResolution;
    Screen.PixelsPerScanline = gop->Mode->Info->PixelsPerScanLine;
    Screen.Format            = info->PixelFormat == PixelRedGreenBlueReserved8BitPerColor   ? FramebufferFormat_ARGB
                               : info->PixelFormat == PixelBlueGreenRedReserved8BitPerColor ? FramebufferFormat_ABGR
                                                                                            : FramebufferFormat_Invalid;

    // need to exit boot services before making an interrupt table
    // status = PrepareInterrupts(SystemTable);
    if (EFI_ERROR(status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Unable to prepare interrupts\r\n");
        return status;
    }

    const Color BackgroundColor = { .r = 0, .g = 0, .b = 51 };
    FillRect(0, 0, Screen.Width, Screen.Height, BackgroundColor);

    const Color TextColor   = { .r = 255, .g = 51, .b = 51 };
    const size_t LeftMargin = 10;
    size_t cursorX          = LeftMargin;
    size_t cursorY          = 20;

    size_t i = 0;
    while (true) {
        char buffer[20];
        PutString(LeftMargin, &cursorX, &cursorY, String_FromLiteral("Iterations: "), BackgroundColor, TextColor);
        PutString(LeftMargin, &cursorX, &cursorY, UInt64ToString(buffer, i), BackgroundColor, TextColor);
        PutString(LeftMargin, &cursorX, &cursorY, String_FromLiteral("\r"), BackgroundColor, TextColor);
        i++;
        asm volatile("hlt");
    }

    return EFI_SUCCESS;
}
