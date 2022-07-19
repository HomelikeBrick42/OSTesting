#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "Memory/EfiMemory.h"

bool PageAllocator_Initialize(EfiMemoryDescriptor* memoryMap, size_t memoryMapSize, size_t memoryMapDescriptorSize);
void PageAllocator_UnreservePages(void* address, size_t count);
void PageAllocator_ReservePages(void* address, size_t count);
void* PageAllocator_AllocatePages(size_t count);
