#include "gnu-efi/inc/efi.h"

EFI_STATUS EFIAPI EfiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE* SystemTable) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Hello World!\n");
    while (1)
        ;
    return EFI_SUCCESS;
}
