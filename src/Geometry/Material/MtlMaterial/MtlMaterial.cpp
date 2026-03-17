#include "MtlMaterial.hpp"

#include "Geometry/Material/MtlMaterial/MtlParameters.hpp"

namespace RTC {
static constexpr float epsilon = 0.001F;

MtlMaterial::MtlMaterial(const MtlParameters& parameters) :
    parameters_(parameters) {}

bool MtlMaterial::scatter(
    const Ray& ray,
    const HitData& hitData,
    Color<float>& attenuation,
    Ray& scatteredRay
) const {
    attenuation = parameters_.diffuse;

    const Point3 reflectedOrigin =
        hitData.hitPoint + epsilon * hitData.hitNormal;

    const Vector3 reflectedDirection =
        ray.getDirection().getReflected(hitData.hitNormal);

    const Ray reflectedRay {reflectedOrigin, reflectedDirection};

    scatteredRay = reflectedRay;
    return true;
}
}