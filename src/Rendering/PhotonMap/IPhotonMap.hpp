#pragma once

#include "Core/Math/Vector.hpp"
#include "Geometry/Hittable/HitData.hpp"
#include "Rendering/PhotonMap/PhotonMaxHeap.hpp"

namespace RTC {
class IPhotonMap {
public:
    IPhotonMap() = default;
    IPhotonMap(const IPhotonMap&) = delete;
    IPhotonMap(IPhotonMap&&) = delete;
    IPhotonMap operator=(const IPhotonMap&) = delete;
    IPhotonMap operator=(IPhotonMap&&) = delete;
    virtual ~IPhotonMap() = default;

    [[nodiscard]] virtual PhotonMaxHeap findNearestPhotons(
        const Vector3f& position,
        uint32_t photonCount
    ) const = 0;

    [[nodiscard]] virtual LinearColor getRadiance(
        const HitData& hitData,
        const Vector3f& outDirection,
        uint32_t photonCount
    ) const = 0;
};
}