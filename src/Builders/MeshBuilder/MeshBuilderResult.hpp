#pragma once

#include "Geometry/Hittable/IHittable.hpp"
#include "Geometry/Light/ILight.hpp"

#include <memory>
#include <vector>

namespace RTC {
struct MeshBuilderResult {
    std::vector<std::unique_ptr<IHittable>> triangles;
    std::vector<std::unique_ptr<ILight>> areaLights;
};
}