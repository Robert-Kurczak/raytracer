#pragma once

#include "Rendering/LightCutsTree/ILightCutsTree.hpp"
#include "Rendering/LightCutsTree/LightNode.hpp"

#include <memory>

namespace RTC {
class LightCutsTree : public ILightCutsTree {
private:
    std::unique_ptr<LightNode> root_;
    std::vector<std::shared_ptr<ILight>> infiniteLights_;

    [[nodiscard]] float estimateClusterError(
        const Point3f& targetPosition,
        const std::unique_ptr<LightNode>& lightNode
    ) const;

    void gatherRecursively(
        const std::unique_ptr<LightNode>& node,
        std::vector<std::shared_ptr<ILight>>& cluster,
        const Point3f& targetPosition,
        float maxError,
        uint32_t maxClusterSize
    ) const;

public:
    LightCutsTree(
        std::unique_ptr<LightNode> root,
        std::vector<std::shared_ptr<ILight>> infiniteLights
    );

    [[nodiscard]] std::vector<std::shared_ptr<ILight>> getCut(
        const Point3f& targetPosition,
        float maxError,
        uint32_t maxClusterSize
    ) const override;
};
}