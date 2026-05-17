#pragma once

#include "Core/Math/Vector.hpp"

namespace RTC {
inline Vector3f transformToWorldSpace(
    const Vector3f& localVector,
    const Vector3f& worldNormal
) {
    const Vector3f helperAxis = std::abs(worldNormal.getX()) > 0.9
                                    ? Vector3f {0.0F, 1.0F, 0.0F}
                                    : Vector3f {1.0F, 0.0F, 0.0F};

    const Vector3f tangent =
        getCrossProduct(worldNormal, helperAxis).getNormalized();

    const Vector3f bitangent = getCrossProduct(tangent, worldNormal);

    const Vector3f globalVersor = tangent * localVector.getX() +
                                  bitangent * localVector.getY() +
                                  worldNormal * localVector.getZ();

    return globalVersor;
}
}