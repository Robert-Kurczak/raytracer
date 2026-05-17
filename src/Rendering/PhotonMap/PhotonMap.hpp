#pragma once

#include "PhotonNode.hpp"
#include "Rendering/PhotonMap/IPhotonMap.hpp"
#include "Rendering/PhotonMap/PhotonMaxHeap.hpp"

namespace RTC {
class PhotonMap : public IPhotonMap {
private:
    std::unique_ptr<PhotonNode> root_;

    static void findRecursively(
        const std::unique_ptr<PhotonNode>& node,
        const Vector3f& position,
        uint32_t photonCount,
        PhotonMaxHeap& maxHeap
    );

public:
    PhotonMap(std::unique_ptr<PhotonNode> root);

    [[nodiscard]] PhotonMaxHeap findNearestPhotons(
        const Vector3f& position,
        uint32_t photonCount
    ) const override;

    [[nodiscard]] LinearColor getRadiance(
        const HitData& hitData,
        const Vector3f& outDirection,
        uint32_t photonCount
    ) const override;
};
}