#pragma once

#include "Geometry/Light/Photon.hpp"

#include <memory>
#include <queue>

namespace RTC {
struct PhotonMaxHeapItem {
    std::shared_ptr<Photon> photon;
    float squaredDistance;

    bool operator<(const PhotonMaxHeapItem& item) const {
        return squaredDistance < item.squaredDistance;
    }
};

using PhotonMaxHeap = std::priority_queue<PhotonMaxHeapItem>;
}