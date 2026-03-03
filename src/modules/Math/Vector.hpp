#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <initializer_list>
#include <ostream>

namespace RTC {
template<typename Type, size_t Size>
class Vector {
public:
    std::array<Type, Size> dimensions {};

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

    [[nodiscard]] constexpr Type getDotProduct(
        const Vector<Type, Size>& vector
    ) const noexcept {
        Type result {0};
        for (size_t i = 0; i < Size; i++) {
            result += dimensions[i] * vector[i];
        }
        return result;
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
};

template<typename Type, size_t Size>
constexpr std::ostream& operator<<(
    std::ostream& out,
    const Vector<Type, Size>& vector
) noexcept {
    out << "Vector<" << int(Size) << "> (";

    for (size_t i = 0; i < Size - 1; i++) {
        out << vector.dimensions[i] << "; ";
    }

    out << vector.dimensions[Size - 1] << ")\n";

    return out;
}

template<typename Type, size_t Size>
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
[[nodiscard]] constexpr Vector<Type, Size> operator*(
    const Vector<Type, Size>& vector,
    const Type& scalar
) noexcept {
    return scalar * vector;
}

template<typename Type, size_t Size>
[[nodiscard]] constexpr Vector<Type, Size> operator/(
    const Vector<Type, Size>& vector,
    const Type& scalar
) noexcept {
    return vector * (1.0F / scalar);
}

template<typename Type>
class Vector2 : public Vector<Type, 2> {
public:
    constexpr Vector2(Type x = Type {}, Type y = Type {}) noexcept :
        Vector<Type, 2>({x, y}) {}

    [[nodiscard]] constexpr Type getX() const noexcept {
        return this->dimensions[0];
    }
    [[nodiscard]] constexpr Type getY() const noexcept {
        return this->dimensions[1];
    }
};

template<typename Type>
class Vector3 : public Vector<Type, 3> {
public:
    constexpr Vector3(
        Type x = Type {},
        Type y = Type {},
        Type z = Type {}
    ) noexcept :
        Vector<Type, 3>({x, y, z}) {}

    [[nodiscard]] constexpr Type getX() const noexcept {
        return this->dimensions[0];
    }
    [[nodiscard]] constexpr Type getY() const noexcept {
        return this->dimensions[1];
    }
    [[nodiscard]] constexpr Type getZ() const noexcept {
        return this->dimensions[2];
    }

    [[nodiscard]] constexpr Vector3<Type> getCrossProduct(
        const Vector3<Type>& vector
    ) const noexcept {
        const Type x = this->dimensions[1] * vector.dimensions[2] -
                       this->dimensions[2] * vector.dimensions[1];

        const Type y = this->dimensions[2] * vector.dimensions[0] -
                       this->dimensions[0] * vector.dimensions[2];

        const Type z = this->dimensions[0] * vector.dimensions[1] -
                       this->dimensions[1] * vector.dimensions[0];

        return Vector3<Type> {x, y, z};
    }
};

template<typename Type>
using Point2 = Vector2<Type>;

template<typename Type>
using Point3 = Vector3<Type>;
}