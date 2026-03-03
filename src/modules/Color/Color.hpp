#pragma once

#include <cstdint>
namespace RTC {
template <typename Type>
struct Color {
    Type red = 0;
    Type green = 0;
    Type blue = 0;
};

using Color8Bit = Color<uint8_t>;
}