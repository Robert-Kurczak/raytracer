#pragma once

#include <array>
#include <cstdint>
#include <initializer_list>

namespace RTC {
template<typename Type, uint8_t Size>
class Vector {
public:
    std::array<Type, Size> dimensions {};

    constexpr Vector() = default;

    constexpr Vector(std::initializer_list<Type> list) {
        for (uint8_t i = 0; i < Size; i++) {
            dimensions[i] = list.begin()[i];
        }
    }

    constexpr Type& operator[](uint32_t index) noexcept {
        return dimensions[index];
    }
    constexpr const Type& operator[](uint32_t index) const noexcept {
        return dimensions[index];
    }

    [[nodiscard]] constexpr Type dot(
        const Vector<Type, Size>& vector
    ) const noexcept {
        Type result {0};
        for (uint8_t i = 0; i < Size; i++) {
            result += dimensions[i] * vector[i];
        }
        return result;
    }
};

template<typename Type>
class Vector2 : public Vector<Type, 2> {
public:
    constexpr Vector2(Type x = Type {}, Type y = Type {}) noexcept {
        this->dimensions[0] = x;
        this->dimensions[1] = y;
    }

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
    ) noexcept {
        this->dimensions[0] = x;
        this->dimensions[1] = y;
        this->dimensions[2] = z;
    }

    [[nodiscard]] constexpr Type getX() const noexcept {
        return this->dimensions[0];
    }
    [[nodiscard]] constexpr Type getY() const noexcept {
        return this->dimensions[1];
    }
    [[nodiscard]] constexpr Type getZ() const noexcept {
        return this->dimensions[2];
    }
};

template<typename Type>
using Point2 = Vector2<Type>;

template<typename Type>
using Point3 = Vector3<Type>;
}