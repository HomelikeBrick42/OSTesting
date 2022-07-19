#include "EFIMemory.h"

String EfiMemoryType_Names[EfiMemoryType_Count] = {
    [EfiMemoryType_ReservedMemoryType]      = String_FromLiteral("EfiReservedMemoryType"),
    [EfiMemoryType_LoaderCode]              = String_FromLiteral("EfiLoaderCode"),
    [EfiMemoryType_LoaderData]              = String_FromLiteral("EfiLoaderData"),
    [EfiMemoryType_BootServicesCode]        = String_FromLiteral("EfiBootServicesCode"),
    [EfiMemoryType_BootServicesData]        = String_FromLiteral("EfiBootServicesData"),
    [EfiMemoryType_RuntimeServicesCode]     = String_FromLiteral("EfiRuntimeServicesCode"),
    [EfiMemoryType_RuntimeServicesData]     = String_FromLiteral("EfiRuntimeServicesData"),
    [EfiMemoryType_ConventionalMemory]      = String_FromLiteral("EfiConventionalMemory"),
    [EfiMemoryType_UnusableMemory]          = String_FromLiteral("EfiUnusableMemory"),
    [EfiMemoryType_ACPIReclaimMemory]       = String_FromLiteral("EfiACPIReclaimMemory"),
    [EfiMemoryType_ACPIMemoryNVS]           = String_FromLiteral("EfiACPIMemoryNVS"),
    [EfiMemoryType_MemoryMappedIO]          = String_FromLiteral("EfiMemoryMappedIO"),
    [EfiMemoryType_MemoryMappedIOPortSpace] = String_FromLiteral("EfiMemoryMappedIOPortSpace"),
    [EfiMemoryType_PalCode]                 = String_FromLiteral("EfiPalCode"),
    [EfiMemotyType_PersistentMemory]        = String_FromLiteral("EfiPersistentMemory"),
};
