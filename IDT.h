#pragma once

#include <stdint.h>

enum {
    IDT_TA_InterruptGate = 0b10001110,
    IDT_TA_CallGate      = 0b10001100,
    IDT_TA_TrapGate      = 0b10001111,
};

typedef struct {
    uint16_t Offset0;
    uint16_t Selector;
    uint8_t IST;
    uint8_t TypesAttributes;
    uint16_t Offset1;
    uint32_t Offset2;
    uint32_t Reserved;
} IDTDescriptorEntry;

void IDTDescriptorEntry_SetOffset(IDTDescriptorEntry* entry, uint64_t value);
uint64_t IDTDescriptorEntry_GetOffset(const IDTDescriptorEntry* entry);

typedef struct {
    uint16_t Limit;
    uint64_t Offset;
} __attribute__((packed)) IDTR;
