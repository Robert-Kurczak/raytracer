#include "DiffuseMaterial.hpp"

#include "Core/Math/Vector.hpp"
#include "Geometry/Material/DiffuseMaterial/DiffuseParameters.hpp"

namespace RTC {
static constexpr float epsilon = 0.001F;

DiffuseParameters DiffuseMaterial::convertFromMtl(
    const MtlParameters& parameters
) const {
    return DiffuseParameters {
        .baseColor = parameters.diffuse,
        .emission = parameters.emission,
        .roughness = std::sqrt(2.0F / (parameters.shininess + 2))
    };
}

DiffuseMaterial::DiffuseMaterial(DiffuseParameters parameters) :
    parameters_(parameters) {}

DiffuseMaterial::DiffuseMaterial(MtlParameters parameters) :
    parameters_(convertFromMtl(parameters)) {}

bool DiffuseMaterial::scatter(
    const Ray& ray,
    const HitData& hitData,
    LinearColor& attenuation,
    Ray& scatteredRay
) const {
    attenuation = parameters_.baseColor;

    const Vector3<float> diffusedDirection =
        Vector3<float>::randomVersorOnHemisphere(hitData.hitNormal);

    const Vector3<float> specularDirection =
        ray.getDirection().getReflected(hitData.hitNormal);

    const Vector3 reflectedDirection =
        interpolateLineary(specularDirection, diffusedDirection, 1);

    const Point3 reflectedOrigin =
        hitData.hitPoint + epsilon * hitData.hitNormal;

    const Ray reflectedRay {reflectedOrigin, reflectedDirection};

    scatteredRay = reflectedRay;
    return true;
}

const LinearColor& DiffuseMaterial::getBaseColor() const {
    return parameters_.baseColor;
}

const LinearColor& DiffuseMaterial::getEmission() const {
    return parameters_.emission;
}
}