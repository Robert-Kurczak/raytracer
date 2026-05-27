#pragma once

#include "Core/Math/Random.hpp"
#include "Core/Math/Vector.hpp"

namespace RTC {
inline Vector3f sampleUnitVector() noexcept {
    const auto polarAngle =
        getRandomNumber<float>(0, 2 * std::numbers::pi);

    const auto azimuthalCos = getRandomNumber<float>(-1.0, 1.0);

    const auto azimuthalSin =
        float(std::sqrt(1.0 - (azimuthalCos * azimuthalCos)));

    return Vector3f {
        azimuthalSin * std::cos(polarAngle),
        azimuthalSin * std::sin(polarAngle),
        azimuthalCos
    };
}

inline Vector3f sampleHemisphere(const Vector3f& normal) noexcept {
    const Vector result = sampleUnitVector();

    if (getDotProduct(normal, result) < 0) {
        return -result;
    }

    return result;
}

inline Vector3f sampleCosineHemisphere() {
    const auto uTheta = getRandomNumber<float>();
    const auto uPhi = getRandomNumber<float>(0, 2 * std::numbers::pi);

    const float sinTheta = std::sqrt(uTheta);

    const float x = sinTheta * std::cos(uPhi);
    const float y = sinTheta * std::sin(uPhi);
    const float z = std::sqrt(1 - uTheta);

    return Vector3f {x, y, z};
}
}