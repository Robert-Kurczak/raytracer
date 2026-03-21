#include "BvhBuilder.hpp"

#include "Geometry/BoundingVolume/AxisAlignedBoundingBox/AxisAlignedBoundingBox.hpp"
#include "Geometry/Hittable/BvhNode/BvhNode.hpp"
#include "Geometry/Hittable/IHittable.hpp"

#include <algorithm>
#include <memory>

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

void BvhBuilder::sortBoxes(
    std::vector<std::unique_ptr<IHittable>>& objects,
    uint32_t rangeStart,
    uint32_t rangeEnd,
    uint32_t depth
) const {
    const uint8_t axisIndex = depth % 3;

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
    uint32_t rangeEnd,
    uint32_t depth
) const {
    sortBoxes(objects, rangeStart, rangeEnd, depth);

    const uint32_t objectSpan = rangeEnd - rangeStart;
    const uint32_t rangeMid = rangeStart + (objectSpan / 2);

    std::unique_ptr<IHittable> leftNode =
        buildRecursively(objects, rangeStart, rangeMid, depth + 1);

    std::unique_ptr<IHittable> rightNode =
        buildRecursively(objects, rangeMid, rangeEnd, depth + 1);

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
    uint32_t rangeEnd,
    uint32_t currentDepth
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

    return createInternalNode(
        objects, rangeStart, rangeEnd, currentDepth
    );
}

[[nodiscard]] std::unique_ptr<IHittable> BvhBuilder::build(
    std::vector<std::unique_ptr<IHittable>>&& objects
) const {
    return buildRecursively(objects, 0, objects.size());
}
}
