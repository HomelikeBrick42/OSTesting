#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
    char* Data;
    size_t Length;
} String;
#define String_FromLiteral(s)               \
    (String) {                              \
        .Data = s, .Length = sizeof(s) - 1, \
    }

String UInt64ToString(char buffer[static 20], uint64_t value);
