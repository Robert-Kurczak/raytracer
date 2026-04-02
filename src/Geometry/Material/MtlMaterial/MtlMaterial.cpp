#include "MtlMaterial.hpp"

#include "Geometry/Material/MtlMaterial/MtlParameters.hpp"

namespace RTC {
MtlMaterial::MtlMaterial(const MtlParameters& parameters) :
    parameters_(parameters) {}

bool MtlMaterial::scatter(
    const Ray& ray,
    const HitData& hitData,
    Color<float>& attenuation,
    Ray& scatteredRay
) const {
    (void) ray;
    (void) hitData;
    (void) scatteredRay;

    attenuation = parameters_.diffuse;

    return false;
}
}