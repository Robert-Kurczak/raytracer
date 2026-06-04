#include "LightCutsTree.hpp"

#include "Geometry/Light/ILight.hpp"

#include <memory>

namespace RTC {
float LightCutsTree::estimateClusterError(
    const Point3f& targetPosition,
    const std::unique_ptr<LightNode>& lightNode
) const {
    return 1.0F; // TODO
}

void LightCutsTree::gatherRecursively(
    const std::unique_ptr<LightNode>& node,
    std::vector<std::shared_ptr<ILight>>& cluster,
    const Point3f& targetPosition,
    float maxError,
    uint32_t maxClusterSize
) const {
    if (!node || cluster.size() == maxClusterSize) {
        return;
    }

    const bool isLeaf = node->left == nullptr and node->right == nullptr;

    if (isLeaf) {
        cluster.push_back(node->representative);
        return;
    }

    const float currentError = estimateClusterError(targetPosition, node);
    const bool errorInRange = currentError <= maxError;

    if (errorInRange) {
        cluster.push_back(node->representative);
        return;
    }

    gatherRecursively(
        node->left, cluster, targetPosition, maxError, maxClusterSize
    );

    gatherRecursively(
        node->right, cluster, targetPosition, maxError, maxClusterSize
    );
}

LightCutsTree::LightCutsTree(
    std::unique_ptr<LightNode> root,
    std::vector<std::shared_ptr<ILight>> infiniteLights
) :
    root_(std::move(root)),
    infiniteLights_(std::move(infiniteLights)) {}

std::vector<std::shared_ptr<ILight>> LightCutsTree::getCut(
    const Point3f& targetPosition,
    float maxError,
    uint32_t maxClusterSize
) const {
    std::vector<std::shared_ptr<ILight>> cluster;

    gatherRecursively(
        root_, cluster, targetPosition, maxError, maxClusterSize
    );

    return cluster;
}

}