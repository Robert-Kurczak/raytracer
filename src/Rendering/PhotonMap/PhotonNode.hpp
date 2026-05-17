#pragma once

#include "Geometry/Light/Photon.hpp"

#include <memory>

namespace RTC {
struct PhotonNode {
    std::shared_ptr<Photon> photon_;
    uint8_t splitAxisIndex_;
    std::unique_ptr<PhotonNode> left_;
    std::unique_ptr<PhotonNode> right_;

    PhotonNode(
        std::shared_ptr<Photon> photon,
        uint8_t splitAxisIndex,
        std::unique_ptr<PhotonNode> left,
        std::unique_ptr<PhotonNode> right
    ) :
        photon_(std::move(photon)),
        splitAxisIndex_(splitAxisIndex),
        left_(std::move(left)),
        right_(std::move(right)) {}
};
}