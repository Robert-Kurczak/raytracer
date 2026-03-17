#pragma once

#include "Geometry/Material/IMaterial.hpp"
#include "Geometry/Material/MtlMaterial/MtlParameters.hpp"

namespace RTC {
class MtlMaterial : public IMaterial {
private:
    MtlParameters parameters_;

public:
    MtlMaterial(const MtlParameters& parameters);

    bool scatter(
        const Ray& ray,
        const HitData& hitData,
        Color<float>& attenuation,
        Ray& scatteredRay
    ) const override;
};
}