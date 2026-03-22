#pragma once

#include "Geometry/Hittable/IHittable.hpp"

#include <filesystem>
#include <memory>
#include <vector>

namespace RTC {
class IMeshBuilder {
public:
    IMeshBuilder() = default;
    IMeshBuilder(const IMeshBuilder&) = delete;
    IMeshBuilder(IMeshBuilder&&) = delete;
    IMeshBuilder operator=(const IMeshBuilder&) = delete;
    IMeshBuilder operator=(IMeshBuilder&&) = delete;
    virtual ~IMeshBuilder() = default;

    using TriangleBuffer = std::vector<std::unique_ptr<IHittable>>;

    [[nodiscard]] virtual TriangleBuffer buildFromFile(
        const std::filesystem::path& path,
        const Vector3<float>& position
    ) const = 0;
};
}