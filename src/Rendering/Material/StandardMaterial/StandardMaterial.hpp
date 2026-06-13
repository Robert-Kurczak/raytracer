#pragma once

#include "Geometry/Hittable/HitData.hpp"
#include "Rendering/Material/IMaterial.hpp"

#include <memory>

namespace RTC {
class StandardMaterial : public IMaterial {
private:
    std::shared_ptr<IMaterial> diffuseMaterial_;
    std::shared_ptr<IMaterial> glossyMaterial_;
    const float glossyBlendFactor_;
    const float diffuseBlendFactor_;

    [[nodiscard]] MaterialSample getGlossySample(
        const HitData& hitData,
        const Vector3f& outDirection
    ) const;

    [[nodiscard]] MaterialSample getDiffuseSample(
        const HitData& hitData,
        const Vector3f& outDirection
    ) const;

public:
    StandardMaterial(
        std::shared_ptr<IMaterial> diffuseMaterial,
        std::shared_ptr<IMaterial> glossyMaterial,
        float glossyBlendFactor
    );

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