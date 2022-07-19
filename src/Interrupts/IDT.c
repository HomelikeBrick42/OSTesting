#include "IDT.h"

void IDTDescriptorEntry_SetOffset(IDTDescriptorEntry* entry, uint64_t value) {
    entry->Offset0 = (uint16_t)((value & 0x000000000000FFFF) >> 0);
    entry->Offset1 = (uint16_t)((value & 0x00000000FFFF0000) >> 16);
    entry->Offset2 = (uint32_t)((value & 0xFFFFFFFF00000000) >> 32);
}

uint64_t IDTDescriptorEntry_GetOffset(const IDTDescriptorEntry* entry) {
    uint64_t offset = 0;
    offset |= (uint64_t)entry->Offset0 << 0;
    offset |= (uint64_t)entry->Offset1 << 16;
    offset |= (uint64_t)entry->Offset2 << 32;
    return offset;
}
