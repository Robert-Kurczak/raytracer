#pragma once

#include <limits>
#include <type_traits>

namespace RTC {
template<typename Type>
requires std::is_arithmetic_v<Type>
struct Interval {
    Type start;
    Type end;

    constexpr Interval(Type start, Type end) noexcept :
        start(start),
        end(end) {}

    constexpr Type getSize() const noexcept {
        return end - start;
    };

    constexpr bool contains(Type value) const noexcept {
        return value >= start && value <= end;
    };

    constexpr bool surrounds(Type value) const noexcept {
        return value > start && value < end;
    }

    constexpr Type clamp(Type value) const noexcept {
        if (value < start) {
            return start;
        }

        if (value > end) {
            return end;
        }

        return value;
    }

    static constexpr Type infinity() noexcept {
        if constexpr (std::is_floating_point_v<Type>) {
            return std::numeric_limits<Type>::infinity();
        } else {
            return std::numeric_limits<Type>::max();
        }
    }

    static constexpr Interval universe() noexcept {
        if constexpr (std::is_signed_v<Type>) {
            return {-infinity(), +infinity()};
        } else {
            return {0, +infinity()};
        }
    }
};
}