#pragma once

#include "Builders/PhotonMapBuilder/IPhotonMapBuilder.hpp"
#include "Geometry/Light/Photon.hpp"
#include "Rendering/PhotonMap/PhotonMap.hpp"
#include "Utils/Logger/ILogger.hpp"

namespace RTC {
class PhotonMapBuilder : public IPhotonMapBuilder {
private:
    std::shared_ptr<ILogger> logger_;

    void sortPhotons(
        std::vector<Photon>& photons,
        uint32_t rangeStart,
        uint32_t rangeEnd,
        uint32_t depth
    ) const;

    [[nodiscard]] std::unique_ptr<PhotonNode> buildRecursively(
        std::vector<Photon>& photons,
        uint32_t rangeStart,
        uint32_t rangeEnd,
        uint32_t depth = 0
    ) const;

public:
    PhotonMapBuilder(std::shared_ptr<ILogger> logger);

    [[nodiscard]] PhotonMap build(
        std::vector<Photon>&& photons
    ) const override;
};
}