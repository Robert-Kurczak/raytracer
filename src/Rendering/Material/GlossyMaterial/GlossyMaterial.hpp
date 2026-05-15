#pragma once

#include "Core/Color/Color.hpp"
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

    [[nodiscard]] Vector3f createMicrofacetNormal() const;

    [[nodiscard]] float getMaskingShadowingLambda(float cosinus) const;

    [[nodiscard]] float getDistributionTerm(float normalsCosinus) const;

    [[nodiscard]] float getGeometricTerm(
        float outCosinus,
        float inCosinus
    ) const;

    [[nodiscard]] LinearColor getFresnelTerm(
        float microfacetOutCosinus
    ) const;

    struct MicrofacetData {
        LinearColor brdf;
        float distribution {};
        float normalsCosinus {};
        float microfacetOutCosinus {};
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

    [[nodiscard]] float calculatePdf(
        const Vector3f& normal,
        const Vector3f& inDirection,
        const Vector3f& outDirection
    ) const override;

    [[nodiscard]] MaterialSample getSample(
        const Point3f& origin,
        const Vector3f& normal,
        const Vector3f& outDirection
    ) const override;
};
}