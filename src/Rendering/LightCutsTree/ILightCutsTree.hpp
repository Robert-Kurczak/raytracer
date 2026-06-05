#pragma once

#include "Rendering/LightCutsTree/LightClusterMaxHeap.hpp"

namespace RTC {
class ILightCutsTree {
public:
    ILightCutsTree() = default;
    ILightCutsTree(const ILightCutsTree&) = delete;
    ILightCutsTree(ILightCutsTree&&) = delete;
    ILightCutsTree operator=(const ILightCutsTree&) = delete;
    ILightCutsTree operator=(ILightCutsTree&&) = delete;
    virtual ~ILightCutsTree() = default;

    [[nodiscard]] virtual LightCutMaxHeap getCut(
        const Point3f& targetPosition,
        float maxError,
        uint32_t maxClusterSize
    ) const = 0;
};
}