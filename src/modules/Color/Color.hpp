#pragma once

#include <cstdint>
#include <ostream>
#include <type_traits>

namespace RTC {
template<typename Type>
requires std::is_arithmetic_v<Type>
struct Color {
    Type red = 0;
    Type green = 0;
    Type blue = 0;

    Color<Type>& operator+=(const Type& scalar) noexcept {
        red += scalar;
        green += scalar;
        blue += scalar;

        return *this;
    }

    Color<Type>& operator-=(const Type& scalar) noexcept {
        red -= scalar;
        green -= scalar;
        blue -= scalar;

        return *this;
    }

    Color<Type>& operator*=(const Type& scalar) noexcept {
        red *= scalar;
        green *= scalar;
        blue *= scalar;

        return *this;
    }

    Color<Type>& operator/=(const Type& scalar) noexcept {
        red /= scalar;
        green /= scalar;
        blue /= scalar;

        return *this;
    }

    Color<Type>& operator+=(const Color<Type>& color) noexcept {
        red += color.red;
        green += color.green;
        blue += color.blue;

        return *this;
    }

    Color<Type>& operator-=(const Color<Type>& color) noexcept {
        red -= color.red;
        green -= color.green;
        blue -= color.blue;

        return *this;
    }

    Color<Type>& operator*=(const Color<Type>& color) noexcept {
        red *= color.red;
        green *= color.green;
        blue *= color.blue;

        return *this;
    }

    Color<Type>& operator/=(const Color<Type>& color) noexcept {
        red /= color.red;
        green /= color.green;
        blue /= color.blue;

        return *this;
    }
};

template<typename Type>
requires std::is_arithmetic_v<Type>
std::ostream& operator<<(std::ostream& out, const Color<Type>& color) {
    out << "Color (" << color.red << "; " << color.green << "; "
        << color.blue << ")";

    return out;
}

template<typename Type>
requires std::is_arithmetic_v<Type>
[[nodiscard]] constexpr Color<Type> operator+(
    const Color<Type>& color1,
    const Color<Type>& color2
) noexcept {
    return Color<Type> {
        color1.red + color2.red,
        color1.green + color2.green,
        color1.blue + color2.blue
    };
}

template<typename Type>
requires std::is_arithmetic_v<Type>
[[nodiscard]] constexpr Color<Type> operator-(
    const Color<Type>& color1,
    const Color<Type>& color2
) noexcept {
    return Color<Type> {
        color1.red - color2.red,
        color1.green - color2.green,
        color1.blue - color2.blue
    };
}

template<typename Type>
requires std::is_arithmetic_v<Type>
[[nodiscard]] constexpr Color<Type> operator*(
    const Color<Type>& color1,
    const Color<Type>& color2
) noexcept {
    return Color<Type> {
        color1.red * color2.red,
        color1.green * color2.green,
        color1.blue * color2.blue
    };
}

template<typename Type>
requires std::is_arithmetic_v<Type>
[[nodiscard]] constexpr Color<Type> operator*(
    const Type& scalar,
    const Color<Type>& color
) noexcept {
    return Color<Type> {
        color.red * scalar, color.green * scalar, color.blue * scalar
    };
}

template<typename Type>
requires std::is_arithmetic_v<Type>
[[nodiscard]] constexpr Color<Type> operator*(
    const Color<Type>& color,
    const Type& scalar
) noexcept {
    return scalar * color;
}

template<typename Type>
requires std::is_arithmetic_v<Type>
[[nodiscard]] constexpr Color<Type> operator/(
    const Color<Type>& color,
    const Type& scalar
) noexcept {
    return Color<Type> {
        color.red / scalar, color.green / scalar, color.blue / scalar
    };
}

using Color8Bit = Color<uint8_t>;
}