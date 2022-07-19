#include "Strings.h"

String UInt64ToString(char buffer[static 20], uint64_t value) {
    size_t length = 0;
    {
        uint64_t temp = value;
        while (temp != 0) {
            temp /= 10;
            length++;
        }
    }
    if (length > 0) {
        for (size_t i = 0; i < length; i++) {
            buffer[length - i - 1] = value % 10 + '0';
            value /= 10;
        }
    } else {
        buffer[0] = '0';
        length++;
    }
    return (String){ .Data = buffer, .Length = length };
}
