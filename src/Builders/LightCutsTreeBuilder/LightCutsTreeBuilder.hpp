#pragma once

#include "Builders/LightCutsTreeBuilder/ILightCutsTreeBuilder.hpp"
#include "Geometry/Light/ILight.hpp"
#include "Utils/Logger/ILogger.hpp"

#include <memory>

namespace RTC {
class LightCutsTreeBuilder : public ILightCutsTreeBuilder {
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

    [[nodiscard]] std::shared_ptr<LightNode> createInternalNode(
        std::vector<std::shared_ptr<ILight>>& lights,
        uint32_t rangeStart,
        uint32_t rangeEnd
    ) const;

    [[nodiscard]] std::shared_ptr<LightNode> buildRecursively(
        std::vector<std::shared_ptr<ILight>>& lights,
        uint32_t rangeStart,
        uint32_t rangeEnd
    ) const;

public:
    LightCutsTreeBuilder(std::shared_ptr<ILogger> logger);

    [[nodiscard]] std::unique_ptr<ILightCutsTree> build(
        std::vector<std::shared_ptr<ILight>>&& lights
    ) const override;
};
}