#include "LightCutsTree.hpp"

#include "Rendering/LightCutsTree/LightClusterMaxHeap.hpp"
#include "Rendering/LightCutsTree/LightNode.hpp"

#include <memory>

namespace RTC {
float LightCutsTree::estimateClusterError(
    const std::shared_ptr<LightNode>& lightNode,
    const Point3f& hitPoint
) const {
    constexpr float visibilityError = 1.0F;

    const float materialError = 1.0F;

    const float distanceSquared =
        lightNode->boundingBox.getDistanceSquared(hitPoint);

    const float geometricError = 1.0F / distanceSquared;

    return lightNode->power.getAverage() * visibilityError *
           materialError * geometricError;
}

LightCutsTree::LightCutsTree(std::shared_ptr<LightNode> root) :
    root_(std::move(root)) {}

LightCutMaxHeap LightCutsTree::getCut(
    const Point3f& hitPoint,
    float maxError,
    uint32_t maxCutSize
) const {
    LightCutMaxHeap maxHeap;

    maxHeap.emplace(
        LightCutMaxHeapItem {
            .lightNode = root_,
            .clusterError = estimateClusterError(root_, hitPoint)
        }
    );

    bool notFull = maxHeap.size() < maxCutSize;
    bool errorTooBig = maxHeap.top().clusterError > maxError;

    while (notFull and errorTooBig) {
        std::shared_ptr<LightNode> highErrorNode =
            maxHeap.top().lightNode;

        if (highErrorNode->isLeaf()) {
            break;
        }

        maxHeap.pop();

        maxHeap.emplace(
            LightCutMaxHeapItem {
                .lightNode = highErrorNode->left,
                .clusterError =
                    estimateClusterError(highErrorNode->left, hitPoint)
            }
        );

        maxHeap.emplace(
            LightCutMaxHeapItem {
                .lightNode = highErrorNode->right,
                .clusterError =
                    estimateClusterError(highErrorNode->right, hitPoint)
            }
        );
    }

    return maxHeap;
}
}