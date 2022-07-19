#pragma once

#include <stddef.h>

#include "Memory/EfiMemory.h"

__attribute__((noreturn)) void KernelMain(EfiMemoryDescriptor* memoryMap, size_t memoryMapSize, size_t memoryMapDescriptorSize);
