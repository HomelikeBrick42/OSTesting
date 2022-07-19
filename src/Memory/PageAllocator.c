#include "PageAllocator.h"
#include "IO/IO.h"

static size_t GetMemorySize(EfiMemoryDescriptor* memoryMap, size_t memoryMapSize, size_t memoryMapDescriptorSize) {
    size_t size = 0;
    for (size_t i = 0; i < (memoryMapSize / memoryMapDescriptorSize); i++) {
        EfiMemoryDescriptor* memoryDescriptor = (EfiMemoryDescriptor*)((size_t)memoryMap + i * memoryMapDescriptorSize);
        size += memoryDescriptor->NumPages * 4096;
    }
    return size;
}

size_t NumPages    = 0;
uint8_t* PageTable = NULL;

bool PageAllocator_Initialize(EfiMemoryDescriptor* memoryMap, size_t memoryMapSize, size_t memoryMapDescriptorSize) {
    size_t memorySize   = GetMemorySize(memoryMap, memoryMapSize, memoryMapDescriptorSize);
    NumPages            = memorySize / 4096;
    size_t requiredSize = NumPages / 8;

    PageTable = NULL;
    {
        void* consecutiveFreePagesStart = NULL;
        size_t consecutiveFreePages     = 0;
        for (size_t i = 0; i < (memoryMapSize / memoryMapDescriptorSize); i++) {
            EfiMemoryDescriptor* memoryDescriptor = (EfiMemoryDescriptor*)((size_t)memoryMap + i * memoryMapDescriptorSize);
            if (memoryDescriptor->Type != EfiMemoryType_ConventionalMemory) {
                consecutiveFreePagesStart = NULL;
                consecutiveFreePages      = 0;
                continue;
            }
            if (consecutiveFreePagesStart == NULL)
                consecutiveFreePagesStart = (void*)memoryDescriptor->PhysicalAddress;
            consecutiveFreePages += memoryDescriptor->NumPages;
            if (consecutiveFreePagesStart == NULL) {
                consecutiveFreePagesStart = (void*)4096; // skip first page
                consecutiveFreePages -= 1;
            }
            if ((consecutiveFreePages * 4096) >= requiredSize) {
                PageTable = consecutiveFreePagesStart;
                break;
            }
        }
    }
    if (PageTable == NULL)
        return false;

    for (size_t i = 0; i < (memoryMapSize / memoryMapDescriptorSize); i++) {
        EfiMemoryDescriptor* memoryDescriptor = (EfiMemoryDescriptor*)((size_t)memoryMap + i * memoryMapDescriptorSize);
        if (memoryDescriptor->Type == EfiMemoryType_ConventionalMemory) {
            PageAllocator_UnreservePages((void*)memoryDescriptor->PhysicalAddress, memoryDescriptor->NumPages);
        } else {
            PageAllocator_ReservePages((void*)memoryDescriptor->PhysicalAddress, memoryDescriptor->NumPages);
        }
    }

    PageAllocator_ReservePages((void*)0, 1);
    PageAllocator_ReservePages(PageTable, requiredSize / 4096);

    return true;
}

void PageAllocator_UnreservePages(void* address, size_t count) {
    for (size_t i = 0; i < count; i++) {
        uint64_t page = (uint64_t)address / 4096 + i;
        if (page > NumPages)
            return;
        PageTable[page / 8] &= ~(1 << (page % 8));
    }
}

void PageAllocator_ReservePages(void* address, size_t count) {
    for (size_t i = 0; i < count; i++) {
        uint64_t page = (uint64_t)address / 4096 + i;
        if (page > NumPages)
            return;
        PageTable[page / 8] |= 1 << (page % 8);
    }
}

void* PageAllocator_AllocatePages(size_t count) {
    void* consecutiveFreePagesStart = NULL;
    size_t consecutiveFreePages     = 0;
    for (size_t i = 0; i < NumPages; i++) {
        if (PageTable[i / 8] & (1 << (i % 8))) {
            consecutiveFreePagesStart = NULL;
            consecutiveFreePages      = 0;
            continue;
        }
        if (consecutiveFreePagesStart == NULL)
            consecutiveFreePagesStart = (void*)(i * 4096);
        consecutiveFreePages++;
        if (consecutiveFreePages >= count) {
            PageAllocator_ReservePages(consecutiveFreePagesStart, count);
            return consecutiveFreePagesStart;
        }
    }
    return NULL;
}
