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

    [[nodiscard]] Vector3f createCosWeightVersor() const;
    [[nodiscard]] Vector3f transformToWorldSpace(
        const Vector3f& localVersor,
        const Vector3f& worldNormal
    ) const;

public:
    DiffuseMaterial(DiffuseParameters parameters);
    DiffuseMaterial(MtlParameters parameters);

    [[nodiscard]] const LinearColor& getBaseColor() const override;

    [[nodiscard]] const LinearColor& getEmission() const override;

    [[nodiscard]] LinearColor getEmission(
        const Point3f& origin,
        const Vector3f& direction
    ) const override;

    [[nodiscard]] LinearColor calculateBrdf(
        const Point3f& origin,
        const Vector3f& outDirection,
        const Vector3f& inDirection
    ) const override;

    [[nodiscard]] MaterialSample getSample(
        const Point3f& origin,
        const Vector3f& normal,
        const Vector3f& outDirection
    ) const override;
};
}