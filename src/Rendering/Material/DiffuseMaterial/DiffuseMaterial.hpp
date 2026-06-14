#pragma once

#include "Core/Color/Color.hpp"
#include "Rendering/Material/DiffuseMaterial/DiffuseParameters.hpp"
#include "Rendering/Material/IMaterial.hpp"

namespace RTC {
class DiffuseMaterial : public IMaterial {
private:
    DiffuseParameters parameters_;

public:
    DiffuseMaterial(const DiffuseParameters& parameters);

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