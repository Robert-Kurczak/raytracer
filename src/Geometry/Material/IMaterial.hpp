#pragma once

#include "Core/Color/Color.hpp"
#include "Core/Ray/Ray.hpp"
#include "Geometry/Hittable/HitData.hpp"

namespace RTC {
class IMaterial {
public:
    IMaterial() = default;
    IMaterial(const IMaterial&) = delete;
    IMaterial(IMaterial&&) = delete;
    IMaterial operator=(const IMaterial&) = delete;
    IMaterial operator=(IMaterial&&) = delete;
    virtual ~IMaterial() = default;

    virtual bool scatter(
        const Ray& ray,
        const HitData& hitData,
        Color<float>& attenuation,
        Ray& scatteredRay
    ) const = 0;
};
}