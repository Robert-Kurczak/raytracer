#include "BvhBuilder.hpp"

#include "Geometry/BoundingVolume/AxisAlignedBoundingBox/AxisAlignedBoundingBox.hpp"
#include "Geometry/Hittable/BvhNode/BvhNode.hpp"
#include "Geometry/Hittable/IHittable.hpp"
#include "Utils/Logger/ILogger.hpp"

#include <algorithm>
#include <chrono>
#include <memory>
#include <utility>

namespace RTC {
bool BvhBuilder::compareBoxes(
    const std::unique_ptr<IHittable>& left,
    const std::unique_ptr<IHittable>& right,
    uint8_t axis
) {
    const float leftCenter =
        left->getBoundingBox().getIntervalCenter(axis);

    const float rightCenter =
        right->getBoundingBox().getIntervalCenter(axis);

    return leftCenter < rightCenter;
}

uint8_t BvhBuilder::getLongestAxisIndex(
    const std::vector<std::unique_ptr<IHittable>>& objects,
    uint32_t rangeStart,
    uint32_t rangeEnd
) const {
    AxisAlignedBoundingBox boundingBox =
        objects[rangeStart]->getBoundingBox();

    for (uint32_t i = rangeStart + 1; i < rangeEnd; i++) {
        boundingBox = AxisAlignedBoundingBox {
            boundingBox, objects[i]->getBoundingBox()
        };
    }

    return boundingBox.getLongestAxisIndex();
}

void BvhBuilder::sortBoxes(
    std::vector<std::unique_ptr<IHittable>>& objects,
    uint32_t rangeStart,
    uint32_t rangeEnd
) const {
    const uint8_t axisIndex =
        getLongestAxisIndex(objects, rangeStart, rangeEnd);

    auto comparator = [axisIndex](const auto& left, const auto& right) {
        return compareBoxes(left, right, axisIndex);
    };

    std::sort(
        objects.begin() + rangeStart,
        objects.begin() + rangeEnd,
        comparator
    );
}

std::unique_ptr<IHittable> BvhBuilder::createInternalNode(
    std::vector<std::unique_ptr<IHittable>>& objects,
    uint32_t rangeStart,
    uint32_t rangeEnd
) const {
    sortBoxes(objects, rangeStart, rangeEnd);

    const uint32_t objectSpan = rangeEnd - rangeStart;
    const uint32_t rangeMid = rangeStart + (objectSpan / 2);

    std::unique_ptr<IHittable> leftNode =
        buildRecursively(objects, rangeStart, rangeMid);

    std::unique_ptr<IHittable> rightNode =
        buildRecursively(objects, rangeMid, rangeEnd);

    const AxisAlignedBoundingBox surroundingBox {
        leftNode->getBoundingBox(), rightNode->getBoundingBox()
    };

    return std::make_unique<BvhNode>(
        std::move(leftNode), std::move(rightNode), surroundingBox
    );
}

std::unique_ptr<IHittable> BvhBuilder::buildRecursively(
    std::vector<std::unique_ptr<IHittable>>& objects,
    uint32_t rangeStart,
    uint32_t rangeEnd
) const {
    const uint32_t objectSpan = rangeEnd - rangeStart;

    if (objectSpan == 1) {
        return std::move(objects[rangeStart]);
    }

    if (objectSpan == 2) {
        std::unique_ptr<IHittable> leftNode =
            std::move(objects[rangeStart]);

        std::unique_ptr<IHittable> rightNode =
            std::move(objects[rangeStart + 1]);

        AxisAlignedBoundingBox surroundingBox(
            leftNode->getBoundingBox(), rightNode->getBoundingBox()
        );

        return std::make_unique<BvhNode>(
            std::move(leftNode), std::move(rightNode), surroundingBox
        );
    }

    return createInternalNode(objects, rangeStart, rangeEnd);
}

BvhBuilder::BvhBuilder(std::shared_ptr<ILogger> logger) :
    logger_(std::move(logger)) {}

[[nodiscard]] std::unique_ptr<IHittable> BvhBuilder::build(
    std::vector<std::unique_ptr<IHittable>>&& objects
) const {
    logger_->log(LogLevel::Info, "Building BVH tree");

    const auto timeStart = std::chrono::high_resolution_clock::now();
    auto result = buildRecursively(objects, 0, objects.size());
    const auto timeEnd = std::chrono::high_resolution_clock::now();

    const auto executionTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            timeEnd - timeStart
        );

    logger_->log(
        LogLevel::Info, std::format("BVH built in {}", executionTime)
    );

    return result;
}
}
