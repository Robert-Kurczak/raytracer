#pragma once

#include "Geometry/Light/ILight.hpp"
#include "Rendering/LightCutsTree/ILightCutsTree.hpp"

#include <memory>
#include <vector>

namespace RTC {
class ILightCutsTreeBuilder {
public:
    ILightCutsTreeBuilder() = default;
    ILightCutsTreeBuilder(const ILightCutsTreeBuilder&) = delete;
    ILightCutsTreeBuilder(ILightCutsTreeBuilder&&) = delete;
    ILightCutsTreeBuilder operator=(const ILightCutsTreeBuilder&) =
        delete;
    ILightCutsTreeBuilder operator=(ILightCutsTreeBuilder&&) = delete;
    virtual ~ILightCutsTreeBuilder() = default;

    [[nodiscard]] virtual std::unique_ptr<ILightCutsTree> build(
        std::vector<std::shared_ptr<ILight>>&& lights
    ) const = 0;
};
}