#pragma once

#include "Geometry/Light/ILight.hpp"
#include "Rendering/LightSampler/LightCutsSampler/LightNode.hpp"

#include <memory>
#include <vector>

namespace RTC {
class ILightTreeBuilder {
public:
    ILightTreeBuilder() = default;
    ILightTreeBuilder(const ILightTreeBuilder&) = delete;
    ILightTreeBuilder(ILightTreeBuilder&&) = delete;
    ILightTreeBuilder operator=(const ILightTreeBuilder&) = delete;
    ILightTreeBuilder operator=(ILightTreeBuilder&&) = delete;
    virtual ~ILightTreeBuilder() = default;

    [[nodiscard]] virtual std::unique_ptr<LightNode> build(
        std::vector<std::shared_ptr<ILight>>&& lights
    ) const = 0;
};
}