#include "Kernel.h"
#include "IO.h"
#include "IDT.h"
#include "Interrupts.h"

#include <stdbool.h>

static IDTR GlobalIDTR;
static void PrepareInterrupts(void) {
    asm volatile("cli");

    GlobalIDTR.Limit  = 0xFFFF;
    GlobalIDTR.Offset = (uint64_t)0; // allocate a page somehow

    IDTDescriptorEntry* pageFault = (IDTDescriptorEntry*)(GlobalIDTR.Offset + 0xE * sizeof(IDTDescriptorEntry));
    IDTDescriptorEntry_SetOffset(pageFault, (uint64_t)PageFault_Handler);
    pageFault->TypesAttributes = IDT_TA_InterruptGate;
    pageFault->Selector        = 0x08;

    asm volatile("lidt %0" : : "m"(GlobalIDTR));

    asm volatile("sti");
}

void KernelMain(void) {
    // need to exit boot services before making an interrupt table
    if (false)
        PrepareInterrupts();

    const Color BackgroundColor = { .r = 0, .g = 0, .b = 51 };
    FillRect(0, 0, Screen.Width, Screen.Height, BackgroundColor);

    const Color TextColor   = { .r = 255, .g = 51, .b = 51 };
    const size_t LeftMargin = 10;
    size_t cursorX          = LeftMargin;
    size_t cursorY          = 20;

    size_t i = 0;
    while (true) {
        char buffer[20];
        PutString(LeftMargin, &cursorX, &cursorY, String_FromLiteral("Interrupts: "), BackgroundColor, TextColor);
        PutString(LeftMargin, &cursorX, &cursorY, UInt64ToString(buffer, i), BackgroundColor, TextColor);
        PutString(LeftMargin, &cursorX, &cursorY, String_FromLiteral("\r"), BackgroundColor, TextColor);
        i++;
        asm volatile("hlt");
    }
}
