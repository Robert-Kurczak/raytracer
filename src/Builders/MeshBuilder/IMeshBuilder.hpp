#pragma once

#include "Builders/MeshBuilder/MeshBuilderResult.hpp"

#include <filesystem>

namespace RTC {
class IMeshBuilder {
public:
    IMeshBuilder() = default;
    IMeshBuilder(const IMeshBuilder&) = delete;
    IMeshBuilder(IMeshBuilder&&) = delete;
    IMeshBuilder operator=(const IMeshBuilder&) = delete;
    IMeshBuilder operator=(IMeshBuilder&&) = delete;
    virtual ~IMeshBuilder() = default;

    [[nodiscard]] virtual MeshBuilderResult buildFromFile(
        const std::filesystem::path& path,
        const Vector3<float>& position
    ) const = 0;
};
}