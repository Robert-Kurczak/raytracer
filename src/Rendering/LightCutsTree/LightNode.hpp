#pragma once

#include "Core/Color/Color.hpp"
#include "Geometry/BoundingVolume/AxisAlignedBoundingBox/AxisAlignedBoundingBox.hpp"
#include "Geometry/Light/ILight.hpp"

#include <memory>
#include <utility>

namespace RTC {
struct LightNode {
    std::shared_ptr<ILight> representative;
    LinearColor power;
    AxisAlignedBoundingBox boundingBox;

    std::unique_ptr<LightNode> left;
    std::unique_ptr<LightNode> right;

    LightNode(
        std::shared_ptr<ILight> representative,
        LinearColor power,
        AxisAlignedBoundingBox boundingBox,
        std::unique_ptr<LightNode> left,
        std::unique_ptr<LightNode> right
    ) :
        representative(std::move(representative)),
        power(power),
        boundingBox(boundingBox),
        left(std::move(left)),
        right(std::move(right)) {}
};
}