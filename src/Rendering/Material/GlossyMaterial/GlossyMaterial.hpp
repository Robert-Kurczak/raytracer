#pragma once

#include "Core/Color/Color.hpp"
#include "Geometry/Hittable/HitData.hpp"
#include "GlossyParameters.hpp"
#include "Rendering/Material/IMaterial.hpp"
#include "Rendering/Material/MtlParameters.hpp"

namespace RTC {
class GlossyMaterial : public IMaterial {
private:
    const LinearColor fresnelBaseTerm_;
    const float alphaSquared_;

    [[nodiscard]] GlossyParameters convertFromMtl(
        const MtlParameters& parameters
    ) const;

    [[nodiscard]] float getAlphaSquared(float roughness) const;

    [[nodiscard]] Vector3f createMicrofacetNormal() const;

    [[nodiscard]] float getMaskingShadowingLambda(float cosine) const;

    [[nodiscard]] float getDistributionTerm(float normalsCosine) const;

    [[nodiscard]] float getGeometricTerm(
        float outCosine,
        float inCosine
    ) const;

    [[nodiscard]] LinearColor getFresnelTerm(
        float microfacetOutCosine
    ) const;

    struct MicrofacetData {
        LinearColor brdf;
        float distribution {};
        float normalsCosine {};
        float microfacetOutCosine {};
    };

    [[nodiscard]] MicrofacetData getCookTorranceTerms(
        const Vector3f& outDirection,
        const Vector3f& inDirection,
        const Vector3f& normal,
        const Vector3f& microfacetNormal
    ) const;

public:
    GlossyMaterial(const GlossyParameters& parameters);
    GlossyMaterial(const MtlParameters& parameters);

    [[nodiscard]] const LinearColor& getEmission() const override;

    [[nodiscard]] LinearColor getEmission(
        const Point3f& origin,
        const Vector3f& direction
    ) const override;

    [[nodiscard]] LinearColor calculateBrdf(
        const HitData& hitData,
        const Vector3f& outDirection,
        const Vector3f& inDirection
    ) const override;

    [[nodiscard]] float calculatePdf(
        const Vector3f& normal,
        const Vector3f& inDirection,
        const Vector3f& outDirection
    ) const override;

    [[nodiscard]] MaterialSample getSample(
        const HitData& hitData,
        const Vector3f& outDirection
    ) const override;
};
}