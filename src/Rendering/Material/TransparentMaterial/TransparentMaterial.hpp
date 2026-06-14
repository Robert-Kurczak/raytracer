#pragma once

#include "Geometry/Hittable/HitData.hpp"
#include "Rendering/Material/IMaterial.hpp"
#include "Rendering/Material/MaterialSample.hpp"
#include "Rendering/Material/TransparentMaterial/TransparentParameters.hpp"

namespace RTC {
class TransparentMaterial : public IMaterial {
private:
    TransparentParameters parameters_;
    const float inverseRefractionIndex_;

    [[nodiscard]] MaterialSample createReflection(
        const Vector3f& outDirection,
        const Vector3f& refractionNormal
    ) const;

    [[nodiscard]] float getFresnelFactor(float incidenceCosine) const;

public:
    TransparentMaterial(const TransparentParameters& parameters);

    [[nodiscard]] const LinearColor& getEmission() const override;

    [[nodiscard]] LinearColor getEmission(
        const Point3f& origin,
        const Vector3f& direction
    ) const override;

    [[nodiscard]] float calculatePdf(
        const Vector3f& normal,
        const Vector3f& inDirection,
        const Vector3f& outDirection
    ) const override;

    [[nodiscard]] LinearColor calculateBrdf(
        const HitData& hitData,
        const Vector3f& outDirection,
        const Vector3f& inDirection
    ) const override;

    [[nodiscard]] MaterialSample getSample(
        const HitData& hitData,
        const Vector3f& outDirection
    ) const override;
};
}