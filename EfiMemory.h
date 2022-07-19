#pragma once

#include <stdint.h>

#include "Strings.h"

enum {
    EfiMemoryType_ReservedMemoryType,
    EfiMemoryType_LoaderCode,
    EfiMemoryType_LoaderData,
    EfiMemoryType_BootServicesCode,
    EfiMemoryType_BootServicesData,
    EfiMemoryType_RuntimeServicesCode,
    EfiMemoryType_RuntimeServicesData,
    EfiMemoryType_ConventionalMemory,
    EfiMemoryType_UnusableMemory,
    EfiMemoryType_ACPIReclaimMemory,
    EfiMemoryType_ACPIMemoryNVS,
    EfiMemoryType_MemoryMappedIO,
    EfiMemoryType_MemoryMappedIOPortSpace,
    EfiMemoryType_PalCode,
    EfiMemotyType_PersistentMemory,
    EfiMemoryType_Count,
};

typedef struct {
    uint32_t Type;
    void* PhysicalAddress;
    void* VirtualAddress;
    uint64_t NumPages;
    uint64_t Attributes;
} EfiMemoryDescriptor;

extern String EfiMemoryType_Names[EfiMemoryType_Count];
