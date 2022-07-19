#pragma once

#include <stddef.h>

__attribute__((noreturn)) void KernelMain(void* memoryMap, size_t memoryMapSize, size_t memoryMapDescriptorSize);
