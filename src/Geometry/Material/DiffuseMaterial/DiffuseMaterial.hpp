#pragma once

#include "Geometry/Material/DiffuseMaterial/DiffuseParameters.hpp"
#include "Geometry/Material/IMaterial.hpp"
#include "Geometry/Material/MtlMaterial/MtlParameters.hpp"

namespace RTC {
class DiffuseMaterial : public IMaterial {
private:
    DiffuseParameters parameters_;

    [[nodiscard]] DiffuseParameters convertFromMtl(
        const MtlParameters& parameters
    ) const;

public:
    DiffuseMaterial(DiffuseParameters parameters);
    DiffuseMaterial(MtlParameters parameters);

    bool scatter(
        const Ray& ray,
        const HitData& hitData,
        Color<float>& attenuation,
        Ray& scatteredRay
    ) const override;
};
}