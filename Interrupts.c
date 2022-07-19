#include "Interrupts.h"
#include "IO.h"

#include <stdbool.h>

void PageFault_Handler(struct interrupt_frame* frame) {
    FillRect(0, 0, Screen.Width, Screen.Height, (Color){ .r = 255, .g = 0, .b = 0 });
    PutString(20,
              &(size_t){ 20 },
              &(size_t){ 30 },
              String_FromLiteral("PAGE FAULT"),
              (Color){ .r = 255, .g = 0, .b = 0 },
              (Color){ .r = 0, .g = 0, .b = 0 });
    while (true) {
        asm volatile("hlt");
    }
}
