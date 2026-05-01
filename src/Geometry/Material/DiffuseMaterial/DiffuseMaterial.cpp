#include "DiffuseMaterial.hpp"

#include "Core/Color/Color.hpp"
#include "Core/Math/Random.hpp"
#include "Core/Math/Vector.hpp"
#include "Geometry/Material/DiffuseMaterial/DiffuseParameters.hpp"
#include "Geometry/Material/MaterialSample.hpp"

#include <numbers>

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

Vector3f DiffuseMaterial::createCosWeightVersor() const {
    const auto uTheta = getRandomNumber<float>();
    const auto uPhi = getRandomNumber<float>(0, 2 * std::numbers::pi);

    const float sinTheta = std::sqrt(uTheta);

    const float x = sinTheta * std::cos(uPhi);
    const float y = sinTheta * std::sin(uPhi);
    const float z = std::sqrt(1 - uTheta);

    return Vector3f {x, y, z};
}

Vector3f DiffuseMaterial::transformToWorldSpace(
    const Vector3f& localVersor,
    const Vector3f& worldNormal
) const {
    const Vector3f helperAxis = std::abs(worldNormal.getX()) > 0.9
                                    ? Vector3f {0.0F, 1.0F, 0.0F}
                                    : Vector3f {1.0F, 0.0F, 0.0F};

    const Vector3f tangent =
        getCrossProduct(worldNormal, helperAxis).getNormalized();

    const Vector3f bitangent = getCrossProduct(tangent, worldNormal);

    const Vector3f globalVersor = tangent * localVersor.getX() +
                                  bitangent * localVersor.getY() +
                                  worldNormal * localVersor.getZ();

    return globalVersor.getNormalized();
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

LinearColor DiffuseMaterial::getEmission(
    const Point3f& origin,
    const Vector3f& direction
) const {
    (void) origin;
    (void) direction;

    return parameters_.emission;
}

LinearColor DiffuseMaterial::calculateBrdf(
    const Point3f& origin,
    const Vector3f& outDirection,
    const Vector3f& inDirection
) const {
    (void) origin;
    (void) outDirection;
    (void) inDirection;

    return parameters_.baseColor / float(std::numbers::pi);
}

MaterialSample DiffuseMaterial::getSample(
    const Point3f& origin,
    const Vector3f& normal,
    const Vector3f& outDirection
) const {
    const Vector3f localVersor = createCosWeightVersor();
    const Vector3f inDirection =
        transformToWorldSpace(localVersor, normal);

    const LinearColor brdf =
        calculateBrdf(origin, outDirection, inDirection);
    const float pdf = localVersor.getZ() / float(std::numbers::pi);

    return MaterialSample {
        .inDirection = inDirection, .brdf = brdf, .pdf = pdf
    };
}
}