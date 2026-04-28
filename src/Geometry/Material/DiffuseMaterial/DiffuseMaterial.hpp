#pragma once

#include "Core/Color/Color.hpp"
#include "Geometry/Material/DiffuseMaterial/DiffuseParameters.hpp"
#include "Geometry/Material/DiffuseMaterial/MtlParameters.hpp"
#include "Geometry/Material/IMaterial.hpp"

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
        LinearColor& attenuation,
        Ray& scatteredRay
    ) const override;

    [[nodiscard]] const LinearColor& getBaseColor() const override;
    [[nodiscard]] const LinearColor& getEmission() const override;
};
}