#pragma once

#include <random>
#include <type_traits>

namespace RTC {
template<typename Type>
requires std::is_floating_point_v<Type>
inline Type getRandomNumber() noexcept {
    static std::mt19937 thread_local randomNumberGenerator {
        std::random_device {}()
    };
    static std::uniform_real_distribution<Type> distribution {0.0F, 1.0F};

    return distribution(randomNumberGenerator);
}

template<typename Type>
requires std::is_floating_point_v<Type>
inline Type getRandomNumber(Type min, Type max) noexcept {
    return min + (getRandomNumber<Type>() * (max - min));
}
}