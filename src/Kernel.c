#include "Kernel.h"
#include "IO/IO.h"
#include "Interrupts/IDT.h"
#include "Interrupts/Interrupts.h"
#include "Memory/EfiMemory.h"
#include "Memory/PageAllocator.h"

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

void KernelMain(EfiMemoryDescriptor* memoryMap, size_t memoryMapSize, size_t memoryMapDescriptorSize) {
    const Color BackgroundColor = { .r = 0, .g = 0, .b = 51 };
    const Color TextColor       = { .r = 255, .g = 51, .b = 51 };
    FillRect(0, 0, Screen.Width, Screen.Height, BackgroundColor);

    if (!PageAllocator_Initialize(memoryMap, memoryMapSize, memoryMapDescriptorSize)) {
        PutString(LeftMargin,
                  &CursorX,
                  &CursorY,
                  String_FromLiteral("Failed to initialize page allocator"),
                  BackgroundColor,
                  TextColor);
        goto end;
    }

    for (size_t i = 0; i < 30; i++) {
        void* page = PageAllocator_AllocatePages(1);
        PutString(LeftMargin, &CursorX, &CursorY, String_FromLiteral("Allocated: "), BackgroundColor, TextColor);
        char buffer[20];
        PutString(LeftMargin, &CursorX, &CursorY, UInt64ToString(buffer, (uint64_t)page), BackgroundColor, TextColor);
        PutString(LeftMargin, &CursorX, &CursorY, String_FromLiteral("\n"), BackgroundColor, TextColor);
        if (i % 2 == 0) {
            PageAllocator_UnreservePages(page, 1);
        }
    }

    // TODO: for when we have a GDT
    if (false)
        PrepareInterrupts();

end:
    while (true) {
        asm volatile("hlt");
    }
}
