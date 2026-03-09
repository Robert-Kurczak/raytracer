#pragma once

#include "RenderEnvironment.hpp"

#include <filesystem>

namespace RTC {
class IEnvironmentBuilder {
public:
    IEnvironmentBuilder() = default;
    IEnvironmentBuilder(const IEnvironmentBuilder&) = delete;
    IEnvironmentBuilder(IEnvironmentBuilder&&) = delete;
    IEnvironmentBuilder operator=(const IEnvironmentBuilder&) = delete;
    IEnvironmentBuilder operator=(IEnvironmentBuilder&&) = delete;
    virtual ~IEnvironmentBuilder() = default;

    [[nodiscard]] virtual RenderEnvironment build(
        const std::filesystem::path& path
    ) const = 0;
};
}