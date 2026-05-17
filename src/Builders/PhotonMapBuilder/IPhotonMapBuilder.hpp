#pragma once

#include "Geometry/Light/Photon.hpp"
#include "Rendering/PhotonMap/PhotonMap.hpp"

#include <vector>

namespace RTC {
class IPhotonMapBuilder {
public:
    IPhotonMapBuilder() = default;
    IPhotonMapBuilder(const IPhotonMapBuilder&) = delete;
    IPhotonMapBuilder(IPhotonMapBuilder&&) = delete;
    IPhotonMapBuilder operator=(const IPhotonMapBuilder&) = delete;
    IPhotonMapBuilder operator=(IPhotonMapBuilder&&) = delete;
    virtual ~IPhotonMapBuilder() = default;

    [[nodiscard]] virtual PhotonMap build(
        std::vector<Photon>&& photons
    ) const = 0;
};
}