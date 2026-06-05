#include "LightCutsTreeBuilder.hpp"

#include "Core/Color/Color.hpp"
#include "Geometry/Light/ILight.hpp"
#include "Rendering/LightCutsTree/ILightCutsTree.hpp"
#include "Rendering/LightCutsTree/LightCutsTree.hpp"
#include "Utils/Logger/ILogger.hpp"

#include <chrono>
#include <memory>

namespace RTC {
bool LightCutsTreeBuilder::compareBoxes(
    const std::shared_ptr<ILight>& left,
    const std::shared_ptr<ILight>& right,
    uint8_t axis
) {
    const float leftCenter =
        left->getBoundingBox().getIntervalCenter(axis);

    const float rightCenter =
        right->getBoundingBox().getIntervalCenter(axis);

    return leftCenter < rightCenter;
}

uint8_t LightCutsTreeBuilder::getLongestAxisIndex(
    const std::vector<std::shared_ptr<ILight>>& lights,
    uint32_t rangeStart,
    uint32_t rangeEnd
) const {
    AxisAlignedBoundingBox boundingBox =
        lights[rangeStart]->getBoundingBox();

    for (uint32_t i = rangeStart + 1; i < rangeEnd; i++) {
        boundingBox = AxisAlignedBoundingBox {
            boundingBox, lights[i]->getBoundingBox()
        };
    }

    return boundingBox.getLongestAxisIndex();
}

void LightCutsTreeBuilder::sortBoxes(
    std::vector<std::shared_ptr<ILight>>& lights,
    uint32_t rangeStart,
    uint32_t rangeEnd
) const {
    const uint8_t axisIndex =
        getLongestAxisIndex(lights, rangeStart, rangeEnd);

    auto comparator = [axisIndex](const auto& left, const auto& right) {
        return compareBoxes(left, right, axisIndex);
    };

    std::sort(
        lights.begin() + rangeStart, lights.begin() + rangeEnd, comparator
    );
}

std::shared_ptr<LightNode> LightCutsTreeBuilder::createInternalNode(
    std::vector<std::shared_ptr<ILight>>& lights,
    uint32_t rangeStart,
    uint32_t rangeEnd
) const {
    sortBoxes(lights, rangeStart, rangeEnd);

    const uint32_t objectSpan = rangeEnd - rangeStart;
    const uint32_t rangeMid = rangeStart + (objectSpan / 2);

    std::shared_ptr<LightNode> leftNode =
        buildRecursively(lights, rangeStart, rangeMid);

    std::shared_ptr<LightNode> rightNode =
        buildRecursively(lights, rangeMid, rangeEnd);

    const AxisAlignedBoundingBox surroundingBox {
        leftNode->boundingBox, rightNode->boundingBox
    };

    std::shared_ptr<ILight> representativeLight = lights[rangeStart];
    float representativePower =
        representativeLight->getPower().getAverage();
    LinearColor cumulativePower = representativeLight->getPower();

    for (uint32_t i = rangeStart + 1; i < rangeEnd; i++) {
        const float power = lights[i]->getPower().getAverage();
        const bool foundGreaterPower = power > representativePower;

        if (foundGreaterPower) {
            representativeLight = lights[i];
            representativePower = power;
        }

        cumulativePower += lights[i]->getPower();
    }

    return std::make_shared<LightNode>(
        representativeLight,
        cumulativePower,
        surroundingBox,
        std::move(leftNode),
        std::move(rightNode)
    );
}

std::shared_ptr<LightNode> LightCutsTreeBuilder::buildRecursively(
    std::vector<std::shared_ptr<ILight>>& lights,
    uint32_t rangeStart,
    uint32_t rangeEnd
) const {
    const uint32_t objectSpan = rangeEnd - rangeStart;

    if (objectSpan == 1) {
        std::shared_ptr<ILight> light = lights[rangeStart];

        return std::make_shared<LightNode>(
            light,
            light->getPower(),
            light->getBoundingBox(),
            nullptr,
            nullptr
        );
    }

    return createInternalNode(lights, rangeStart, rangeEnd);
}

LightCutsTreeBuilder::LightCutsTreeBuilder(
    std::shared_ptr<ILogger> logger
) :
    logger_(std::move(logger)) {}

std::unique_ptr<ILightCutsTree> LightCutsTreeBuilder::build(
    std::vector<std::shared_ptr<ILight>>&& lights
) const {
    logger_->log(LogLevel::Info, "Building Light Cuts tree");

    const auto timeStart = std::chrono::high_resolution_clock::now();
    auto result = buildRecursively(lights, 0, lights.size());
    const auto timeEnd = std::chrono::high_resolution_clock::now();

    const auto executionTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            timeEnd - timeStart
        );

    logger_->log(
        LogLevel::Info,
        std::format("Light Cuts built in {}", executionTime)
    );

    return std::make_unique<LightCutsTree>(std::move(result));
}
}