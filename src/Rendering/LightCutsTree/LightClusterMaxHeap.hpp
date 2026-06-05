#pragma once

#include "Rendering/LightCutsTree/LightNode.hpp"

#include <memory>
#include <queue>

namespace RTC {
struct LightCutMaxHeapItem {
    std::shared_ptr<LightNode> lightNode;
    float clusterError;

    bool operator<(const LightCutMaxHeapItem& item) const {
        return clusterError < item.clusterError;
    }
};

using LightCutMaxHeap = std::priority_queue<LightCutMaxHeapItem>;
}