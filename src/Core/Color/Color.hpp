#pragma once

#include <cstdint>
#include <ostream>
#include <type_traits>

namespace RTC {
template<typename Type, typename Tag>
requires std::is_arithmetic_v<Type>
struct Color {
    Type red = 0;
    Type green = 0;
    Type blue = 0;

    static constexpr Color black() noexcept {
        return Color {Type {0}, Type {0}, Type {0}};
    }

    [[nodiscard]] bool isBlack() const noexcept {
        constexpr float epsilon = 1e-6;
        return red + green + blue <= epsilon;
    }

    Color& operator*=(const Type& scalar) noexcept {
        red *= scalar;
        green *= scalar;
        blue *= scalar;

        return *this;
    }

    Color& operator/=(const Type& scalar) noexcept {
        red /= scalar;
        green /= scalar;
        blue /= scalar;

        return *this;
    }

    Color& operator+=(const Color& color) noexcept {
        red += color.red;
        green += color.green;
        blue += color.blue;

        return *this;
    }

    Color& operator-=(const Color& color) noexcept {
        red -= color.red;
        green -= color.green;
        blue -= color.blue;

        return *this;
    }
};

template<typename Type, typename Tag>
requires std::is_arithmetic_v<Type>
std::ostream& operator<<(
    std::ostream& out,
    const Color<Type, Tag>& color
) {
    out << "Color (" << color.red << "; " << color.green << "; "
        << color.blue << ")";

    return out;
}

template<typename Type, typename Tag>
requires std::is_arithmetic_v<Type>
[[nodiscard]] constexpr Color<Type, Tag> operator+(
    const Color<Type, Tag>& color1,
    const Color<Type, Tag>& color2
) noexcept {
    return Color<Type, Tag> {
        color1.red + color2.red,
        color1.green + color2.green,
        color1.blue + color2.blue
    };
}

template<typename Type, typename Tag>
requires std::is_arithmetic_v<Type>
[[nodiscard]] constexpr Color<Type, Tag> operator-(
    const Color<Type, Tag>& color1,
    const Color<Type, Tag>& color2
) noexcept {
    return Color<Type, Tag> {
        color1.red - color2.red,
        color1.green - color2.green,
        color1.blue - color2.blue
    };
}

template<typename Type, typename Tag>
requires std::is_arithmetic_v<Type>
[[nodiscard]] constexpr Color<Type, Tag> operator*(
    const Color<Type, Tag>& color1,
    const Color<Type, Tag>& color2
) noexcept {
    return Color<Type, Tag> {
        color1.red * color2.red,
        color1.green * color2.green,
        color1.blue * color2.blue
    };
}

template<typename Type, typename Tag>
requires std::is_arithmetic_v<Type>
[[nodiscard]] constexpr bool operator==(
    const Color<Type, Tag>& color1,
    const Color<Type, Tag>& color2
) noexcept {
    return color1.red == color2.red and color1.green == color2.green and
           color1.blue == color2.blue;
}

template<typename Type, typename Tag>
requires std::is_arithmetic_v<Type>
[[nodiscard]] constexpr Color<Type, Tag> operator*(
    const Type& scalar,
    const Color<Type, Tag>& color
) noexcept {
    return Color<Type, Tag> {
        color.red * scalar, color.green * scalar, color.blue * scalar
    };
}

template<typename Type, typename Tag>
requires std::is_arithmetic_v<Type>
[[nodiscard]] constexpr Color<Type, Tag> operator*(
    const Color<Type, Tag>& color,
    const Type& scalar
) noexcept {
    return scalar * color;
}

template<typename Type, typename Tag>
requires std::is_arithmetic_v<Type>
[[nodiscard]] constexpr Color<Type, Tag> operator/(
    const Color<Type, Tag>& color,
    const Type& scalar
) noexcept {
    return Color<Type, Tag> {
        color.red / scalar, color.green / scalar, color.blue / scalar
    };
}

namespace Tags {
struct LinearColorTag {};
struct Color8BitTag {};
}

using LinearColor = Color<float, Tags::LinearColorTag>;
using Color8Bit = Color<uint8_t, Tags::Color8BitTag>;

inline constexpr LinearColor BLACK_LINEAR_COLOR {
    .red = 0.0F,
    .green = 0.0F,
    .blue = 0.0F
};

inline constexpr LinearColor UNIT_LINEAR_COLOR {
    .red = 1.0F,
    .green = 1.0F,
    .blue = 1.0F
};
}