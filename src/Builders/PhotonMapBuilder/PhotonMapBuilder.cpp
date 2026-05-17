#include "PhotonMapBuilder.hpp"

#include <memory>
#include <utility>

namespace RTC {
void PhotonMapBuilder::sortPhotons(
    std::vector<Photon>& photons,
    uint32_t rangeStart,
    uint32_t rangeEnd,
    uint32_t depth
) const {
    const uint8_t axisIndex = depth % 3;

    auto comparator =
        [axisIndex](const Photon& left, const Photon& right) {
            return left.position[axisIndex] < right.position[axisIndex];
        };

    std::sort(
        photons.begin() + rangeStart,
        photons.begin() + rangeEnd,
        comparator
    );
}

std::unique_ptr<PhotonNode> PhotonMapBuilder::buildRecursively(
    std::vector<Photon>& photons,
    uint32_t rangeStart,
    uint32_t rangeEnd,
    uint32_t depth
) const {
    const uint32_t objectSpan = rangeEnd - rangeStart;

    if (rangeStart >= rangeEnd) {
        return nullptr;
    }

    sortPhotons(photons, rangeStart, rangeEnd, depth);

    const uint32_t rangeMid = rangeStart + (objectSpan / 2);

    std::unique_ptr<PhotonNode> leftNode =
        buildRecursively(photons, rangeStart, rangeMid, depth + 1);

    std::unique_ptr<PhotonNode> rightNode =
        buildRecursively(photons, rangeMid + 1, rangeEnd, depth + 1);

    return std::make_unique<PhotonNode>(
        std::make_shared<Photon>(photons[rangeMid]),
        depth % 3,
        std::move(leftNode),
        std::move(rightNode)
    );
}

PhotonMapBuilder::PhotonMapBuilder(std::shared_ptr<ILogger> logger) :
    logger_(std::move(logger)) {}

PhotonMap PhotonMapBuilder::build(std::vector<Photon>&& photons) const {
    std::unique_ptr<PhotonNode> treeRoot =
        buildRecursively(photons, 0, photons.size());

    return PhotonMap {std::move(treeRoot)};
}
}