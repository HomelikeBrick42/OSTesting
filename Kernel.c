#include "Kernel.h"
#include "IO.h"
#include "IDT.h"
#include "Interrupts.h"
#include "EfiMemory.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

static IDTR GlobalIDTR;
static IDTDescriptorEntry IDTEntries[256];
static void PrepareInterrupts(void) {
    asm volatile("cli");

    GlobalIDTR.Limit  = 0xFFFF;
    GlobalIDTR.Offset = (uint64_t)IDTEntries;

    IDTDescriptorEntry* pageFault = &IDTEntries[0xE];
    IDTDescriptorEntry_SetOffset(pageFault, (uint64_t)PageFault_Handler);
    pageFault->Selector        = 0x08;
    pageFault->IST             = 0;
    pageFault->TypesAttributes = IDT_TA_InterruptGate;

    asm volatile("lidt %0" : : "m"(GlobalIDTR));

    asm volatile("sti");
}

void KernelMain(void* memoryMap, size_t memoryMapSize, size_t memoryMapDescriptorSize) {
    const Color BackgroundColor = { .r = 0, .g = 0, .b = 51 };
    FillRect(0, 0, Screen.Width, Screen.Height, BackgroundColor);

    const Color TextColor   = { .r = 255, .g = 51, .b = 51 };
    const size_t LeftMargin = 10;
    size_t cursorX          = LeftMargin;
    size_t cursorY          = 20;

    char buffer[20];
    PutString(LeftMargin, &cursorX, &cursorY, String_FromLiteral("Memory Map: "), BackgroundColor, TextColor);
    PutString(LeftMargin, &cursorX, &cursorY, UInt64ToString(buffer, (uint64_t)memoryMap), BackgroundColor, TextColor);
    PutString(LeftMargin, &cursorX, &cursorY, String_FromLiteral("\n"), BackgroundColor, TextColor);

    PutString(LeftMargin, &cursorX, &cursorY, String_FromLiteral("Memory Map Size: "), BackgroundColor, TextColor);
    PutString(LeftMargin, &cursorX, &cursorY, UInt64ToString(buffer, memoryMapSize), BackgroundColor, TextColor);
    PutString(LeftMargin, &cursorX, &cursorY, String_FromLiteral("\n"), BackgroundColor, TextColor);

    PutString(LeftMargin, &cursorX, &cursorY, String_FromLiteral("Memory Map Descriptor Size: "), BackgroundColor, TextColor);
    PutString(LeftMargin, &cursorX, &cursorY, UInt64ToString(buffer, memoryMapDescriptorSize), BackgroundColor, TextColor);
    PutString(LeftMargin, &cursorX, &cursorY, String_FromLiteral("\n"), BackgroundColor, TextColor);

    {
        size_t memoryMapEntries = memoryMapSize / memoryMapDescriptorSize;
        for (size_t i = 0; i < memoryMapEntries; i++) {
            EfiMemoryDescriptor* descriptor = (EfiMemoryDescriptor*)((uint64_t)memoryMap + i * memoryMapDescriptorSize);

            PutString(LeftMargin, &cursorX, &cursorY, String_FromLiteral("Memory Type: "), BackgroundColor, TextColor);
            PutString(LeftMargin, &cursorX, &cursorY, UInt64ToString(buffer, descriptor->Type), BackgroundColor, TextColor);
            PutString(LeftMargin, &cursorX, &cursorY, String_FromLiteral("\n"), BackgroundColor, TextColor);
        }
    }

    if (false)
        PrepareInterrupts();

    while (true) {
        asm volatile("hlt");
    }
}
