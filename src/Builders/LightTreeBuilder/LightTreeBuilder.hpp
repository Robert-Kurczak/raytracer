#pragma once

#include "Builders/LightTreeBuilder/ILightTreeBuilder.hpp"
#include "Geometry/Light/ILight.hpp"
#include "Rendering/LightSampler/LightCutsSampler/LightNode.hpp"
#include "Utils/Logger/ILogger.hpp"

#include <memory>

namespace RTC {
class LightTreeBuilder : public ILightTreeBuilder {
private:
    std::shared_ptr<ILogger> logger_;

    [[nodiscard]] static bool compareBoxes(
        const std::shared_ptr<ILight>& left,
        const std::shared_ptr<ILight>& right,
        uint8_t axis
    );

    [[nodiscard]] uint8_t getLongestAxisIndex(
        const std::vector<std::shared_ptr<ILight>>& lights,
        uint32_t rangeStart,
        uint32_t rangeEnd
    ) const;

    void sortBoxes(
        std::vector<std::shared_ptr<ILight>>& lights,
        uint32_t rangeStart,
        uint32_t rangeEnd
    ) const;

    [[nodiscard]] std::unique_ptr<LightNode> createInternalNode(
        std::vector<std::shared_ptr<ILight>>& lights,
        uint32_t rangeStart,
        uint32_t rangeEnd
    ) const;

    [[nodiscard]] std::unique_ptr<LightNode> buildRecursively(
        std::vector<std::shared_ptr<ILight>>& lights,
        uint32_t rangeStart,
        uint32_t rangeEnd
    ) const;

public:
    LightTreeBuilder(std::shared_ptr<ILogger> logger);

    [[nodiscard]] std::unique_ptr<LightNode> build(
        std::vector<std::shared_ptr<ILight>>&& lights
    ) const override;
};
}