#pragma once

#include "Geometry/Light/ILight.hpp"

#include <memory>
#include <vector>

namespace RTC {
class ILightCutsTree {
public:
    ILightCutsTree() = default;
    ILightCutsTree(const ILightCutsTree&) = delete;
    ILightCutsTree(ILightCutsTree&&) = delete;
    ILightCutsTree operator=(const ILightCutsTree&) = delete;
    ILightCutsTree operator=(ILightCutsTree&&) = delete;
    virtual ~ILightCutsTree() = default;

    [[nodiscard]] virtual std::vector<std::shared_ptr<ILight>> getCut(
        const Point3f& targetPosition,
        float maxError,
        uint32_t maxClusterSize
    ) const;
};
}