#pragma once

#include "Geometry/Light/ILight.hpp"

#include <memory>
#include <utility>

namespace RTC {
struct LightNode {
    std::shared_ptr<ILight> representative;
    LinearColor power;
    AxisAlignedBoundingBox boundingBox;

    std::shared_ptr<LightNode> left;
    std::shared_ptr<LightNode> right;

    [[nodiscard]] bool isLeaf() const {
        return left == nullptr and right == nullptr;
    }

    LightNode(
        std::shared_ptr<ILight> representative,
        LinearColor power,
        AxisAlignedBoundingBox boundingBox,
        std::shared_ptr<LightNode> left,
        std::shared_ptr<LightNode> right
    ) :
        representative(std::move(representative)),
        power(power),
        boundingBox(boundingBox),
        left(std::move(left)),
        right(std::move(right)) {}
};
}