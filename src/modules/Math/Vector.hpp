#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <initializer_list>
#include <ostream>

namespace RTC {
template<typename Type, size_t Size>
requires std::is_arithmetic_v<Type>
class Vector {
private:
    std::array<Type, Size> dimensions {};

public:
    constexpr Vector() noexcept = default;

    constexpr Vector(std::initializer_list<Type> list) noexcept {
        const size_t elementsAmount = std::min(Size, list.size());

        for (size_t i = 0; i < elementsAmount; i++) {
            dimensions[i] = list.begin()[i];
        }
    }

    [[nodiscard]] constexpr Type& operator[](size_t index) noexcept {
        return dimensions[index];
    }
    [[nodiscard]] constexpr const Type& operator[](
        size_t index
    ) const noexcept {
        return dimensions[index];
    }

    Vector<Type, Size>& operator+=(
        const Vector<Type, Size>& vector
    ) noexcept {
        for (size_t i = 0; i < Size; i++) {
            dimensions[i] += vector[i];
        }

        return *this;
    }

    Vector<Type, Size>& operator-=(
        const Vector<Type, Size>& vector
    ) noexcept {
        for (size_t i = 0; i < Size; i++) {
            dimensions[i] -= vector[i];
        }

        return *this;
    }

    Vector<Type, Size>& operator*=(const Type& scalar) noexcept {
        for (size_t i = 0; i < Size; i++) {
            dimensions[i] *= scalar;
        }

        return *this;
    }

    Vector<Type, Size>& operator/=(const Type& scalar) noexcept {
        for (size_t i = 0; i < Size; i++) {
            dimensions[i] /= scalar;
        }

        return *this;
    }

    [[nodiscard]] constexpr Vector<Type, Size> operator-() const noexcept {
        Vector<Type, Size> resultVector {};

        for (size_t i = 0; i < Size; i++) {
            resultVector[i] = -dimensions[i];
        }

        return resultVector;
    }

    [[nodiscard]] constexpr Type getSquaredLength() const noexcept {
        Type result {};
        for (size_t i = 0; i < Size; i++) {
            result += dimensions[i] * dimensions[i];
        }
        return result;
    }

    [[nodiscard]] constexpr Type getLength() const noexcept {
        return std::sqrt(getSquaredLength());
    }

    [[nodiscard]] constexpr Vector<Type, Size> getVersor() const noexcept {
        return *this / getLength();
    }

    [[nodiscard]] constexpr const Type& getX() const noexcept
        requires(Size >= 1)
    {
        return dimensions[0];
    }

    [[nodiscard]] constexpr const Type& getY() const noexcept
        requires(Size >= 2)
    {
        return dimensions[1];
    }

    [[nodiscard]] constexpr const Type& getZ() const noexcept
        requires(Size >= 3)
    {
        return dimensions[2];
    }
};

template<typename Type, size_t Size>
requires std::is_arithmetic_v<Type>
std::ostream& operator<<(
    std::ostream& out,
    const Vector<Type, Size>& vector
) {
    out << "Vector<" << int(Size) << "> (";

    for (size_t i = 0; i < Size - 1; i++) {
        out << vector[i] << "; ";
    }

    out << vector[Size - 1] << ")\n";

    return out;
}

template<typename Type, size_t Size>
requires std::is_arithmetic_v<Type>
[[nodiscard]] constexpr Vector<Type, Size> operator+(
    const Vector<Type, Size>& vector1,
    const Vector<Type, Size>& vector2
) noexcept {
    Vector<Type, Size> resultVector {};

    for (size_t i = 0; i < Size; i++) {
        resultVector[i] = vector1[i] + vector2[i];
    }

    return resultVector;
}

template<typename Type, size_t Size>
requires std::is_arithmetic_v<Type>
[[nodiscard]] constexpr Vector<Type, Size> operator-(
    const Vector<Type, Size>& vector1,
    const Vector<Type, Size>& vector2
) noexcept {
    Vector<Type, Size> resultVector {};

    for (size_t i = 0; i < Size; i++) {
        resultVector[i] = vector1[i] - vector2[i];
    }

    return resultVector;
}

template<typename Type, size_t Size>
requires std::is_arithmetic_v<Type>
[[nodiscard]] constexpr Vector<Type, Size> operator*(
    const Vector<Type, Size>& vector1,
    const Vector<Type, Size>& vector2
) noexcept {
    Vector<Type, Size> resultVector {};

    for (size_t i = 0; i < Size; i++) {
        resultVector[i] = vector1[i] * vector2[i];
    }

    return resultVector;
}

template<typename Type, size_t Size>
requires std::is_arithmetic_v<Type>
[[nodiscard]] constexpr Vector<Type, Size> operator*(
    const Type& scalar,
    const Vector<Type, Size>& vector
) noexcept {
    Vector<Type, Size> resultVector {};

    for (size_t i = 0; i < Size; i++) {
        resultVector[i] = scalar * vector[i];
    }

    return resultVector;
}

template<typename Type, size_t Size>
requires std::is_arithmetic_v<Type>
[[nodiscard]] constexpr Vector<Type, Size> operator*(
    const Vector<Type, Size>& vector,
    const Type& scalar
) noexcept {
    return scalar * vector;
}

template<typename Type, size_t Size>
requires std::is_arithmetic_v<Type>
[[nodiscard]] constexpr Vector<Type, Size> operator/(
    const Vector<Type, Size>& vector,
    const Type& scalar
) noexcept {
    Vector<Type, Size> resultVector {};

    for (size_t i = 0; i < Size; i++) {
        resultVector[i] = vector[i] / scalar;
    }

    return resultVector;
}

template<typename Type, size_t Size>
requires std::is_arithmetic_v<Type>
[[nodiscard]] constexpr Type getDotProduct(
    const Vector<Type, Size>& vector1,
    const Vector<Type, Size>& vector2
) noexcept {
    Type result {0};
    for (size_t i = 0; i < Size; i++) {
        result += vector1[i] * vector2[i];
    }
    return result;
}

template<typename Type>
requires std::is_arithmetic_v<Type>
[[nodiscard]] constexpr Vector<Type, 3> getCrossProduct(
    const Vector<Type, 3>& vector1,
    const Vector<Type, 3>& vector2
) noexcept {
    const Type x = vector1[1] * vector2[2] - vector1[2] * vector2[1];
    const Type y = vector1[2] * vector2[0] - vector1[0] * vector2[2];
    const Type z = vector1[0] * vector2[1] - vector1[1] * vector2[0];

    return Vector<Type, 3> {x, y, z};
};

template<typename Type>
requires std::is_arithmetic_v<Type>
using Vector3 = Vector<Type, 3>;

template<typename Type>
requires std::is_arithmetic_v<Type>
using Vector2 = Vector<Type, 2>;
}