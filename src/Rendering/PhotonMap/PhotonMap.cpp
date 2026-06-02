#include "PhotonMap.hpp"

#include "Core/Math/Numeric.hpp"
#include "Rendering/Material/IMaterial.hpp"
#include "Rendering/PhotonMap/PhotonMaxHeap.hpp"

namespace RTC {
void PhotonMap::findRecursively(
    const std::unique_ptr<PhotonNode>& node,
    const Vector3f& position,
    uint32_t photonCount,
    PhotonMaxHeap& maxHeap
) {
    if (node == nullptr) {
        return;
    }

    const float squaredDistance =
        (node->photon_->position - position).getSquaredLength();

    const PhotonMaxHeapItem newItem {
        .photon = node->photon_, .squaredDistance = squaredDistance
    };

    if (maxHeap.size() < photonCount) {
        maxHeap.push(newItem);
    } else if (maxHeap.top().squaredDistance > squaredDistance) {
        maxHeap.pop();
        maxHeap.push(newItem);
    }

    const uint8_t splitAxisIndex = node->splitAxisIndex_;

    const float distanceToSplitPlane =
        position[splitAxisIndex] -
        node->photon_->position[splitAxisIndex];

    const float squaredDistanceToSplitPlane =
        distanceToSplitPlane * distanceToSplitPlane;

    const std::unique_ptr<PhotonNode>& first =
        distanceToSplitPlane < 0 ? node->left_ : node->right_;

    const std::unique_ptr<PhotonNode>& second =
        distanceToSplitPlane < 0 ? node->right_ : node->left_;

    findRecursively(first, position, photonCount, maxHeap);

    const bool nonFilledHeap = maxHeap.size() < photonCount;
    const bool foundCloserNeighbor =
        squaredDistanceToSplitPlane < maxHeap.top().squaredDistance;

    if (nonFilledHeap or foundCloserNeighbor) {
        findRecursively(second, position, photonCount, maxHeap);
    }
}

PhotonMap::PhotonMap(std::unique_ptr<PhotonNode> root) :
    root_(std::move(root)) {}

PhotonMaxHeap PhotonMap::findNearestPhotons(
    const Vector3f& position,
    uint32_t photonCount
) const {
    PhotonMaxHeap maxHeap;

    findRecursively(root_, position, photonCount, maxHeap);

    return maxHeap;
}

LinearColor PhotonMap::getRadiance(
    const HitData& hitData,
    const Vector3f& outDirection,
    uint32_t photonCount
) const {
    PhotonMaxHeap neighbors =
        findNearestPhotons(hitData.hitPoint, photonCount);

    if (neighbors.empty()) {
        return LinearColor::black();
    }

    const float squaredMaxRadius =
        std::max(EPSILON, neighbors.top().squaredDistance);

    const auto area = float(std::numbers::pi * squaredMaxRadius);

    LinearColor radiance = LinearColor::black();

    while (not neighbors.empty()) {
        const LinearColor brdf = hitData.material->calculateBrdf(
            hitData.hitPoint,
            hitData.hitNormal,
            outDirection,
            -neighbors.top().photon->direction.getNormalized()
        );

        radiance += brdf * neighbors.top().photon->power / area;
        neighbors.pop();
    }

    return radiance;
}
}