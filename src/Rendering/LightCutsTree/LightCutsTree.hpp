#pragma once

#include "Rendering/LightCutsTree/ILightCutsTree.hpp"
#include "Rendering/LightCutsTree/LightClusterMaxHeap.hpp"
#include "Rendering/LightCutsTree/LightNode.hpp"

#include <memory>

namespace RTC {
class LightCutsTree : public ILightCutsTree {
private:
    std::shared_ptr<LightNode> root_;
    std::vector<std::shared_ptr<ILight>> infiniteLights_;

    [[nodiscard]] float estimateClusterError(
        const std::shared_ptr<LightNode>& lightNode,
        const Point3f& hitPoint
    ) const;

public:
    LightCutsTree(std::shared_ptr<LightNode> root);

    [[nodiscard]] LightCutMaxHeap getCut(
        const Point3f& hitPoint,
        float maxError,
        uint32_t maxCutSize
    ) const override;
};
}