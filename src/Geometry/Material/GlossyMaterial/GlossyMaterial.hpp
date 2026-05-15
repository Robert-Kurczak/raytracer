#pragma once

#include "Core/Color/Color.hpp"
#include "Geometry/Material/IMaterial.hpp"
#include "Geometry/Material/MtlParameters.hpp"
#include "GlossyParameters.hpp"

namespace RTC {
class GlossyMaterial : public IMaterial {
private:
    GlossyParameters parameters_;

    const float alphaSquared_;

    [[nodiscard]] GlossyParameters convertFromMtl(
        const MtlParameters& parameters
    ) const;

    [[nodiscard]] Vector3f createMicrofacetNormal() const;
    [[nodiscard]] Vector3f transformToWorldSpace(
        const Vector3f& localVersor,
        const Vector3f& worldNormal
    ) const;

    [[nodiscard]] float getDistributionTerm(
        const Vector3f& microfacetNormal,
        const Vector3f& macrosurfaceNormal
    ) const;

    [[nodiscard]] float getGeometricTerm(
        const Vector3f& outDirection,
        const Vector3f& inDirection,
        const Vector3f& macrosurfaceNormal
    ) const;

    [[nodiscard]] LinearColor getFresnelTerm(
        const Vector3f& outDirection,
        const Vector3f& microfacetNormal
    ) const;

public:
    GlossyMaterial(const MtlParameters& parameters);

    [[nodiscard]] const LinearColor& getBaseColor() const override;

    [[nodiscard]] const LinearColor& getEmission() const override;

    [[nodiscard]] LinearColor getEmission(
        const Point3f& origin,
        const Vector3f& direction
    ) const override;

    [[nodiscard]] LinearColor calculateBrdf(
        const Point3f& origin,
        const Vector3f& normal,
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