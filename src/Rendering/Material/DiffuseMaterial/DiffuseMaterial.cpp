#include "DiffuseMaterial.hpp"

#include "Core/Color/Color.hpp"
#include "Core/Math/Numeric.hpp"
#include "Core/Math/Random.hpp"
#include "Core/Math/Transformations.hpp"
#include "Core/Math/Vector.hpp"
#include "Rendering/Material/DiffuseMaterial/DiffuseParameters.hpp"
#include "Rendering/Material/MaterialSample.hpp"

#include <numbers>

namespace RTC {

DiffuseParameters DiffuseMaterial::convertFromMtl(
    const MtlParameters& parameters
) const {
    return DiffuseParameters {
        .baseColor = parameters.diffuse,
        .emission = parameters.emission / float(std::numbers::pi)
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

DiffuseMaterial::DiffuseMaterial(const DiffuseParameters& parameters) :
    parameters_(parameters) {}

DiffuseMaterial::DiffuseMaterial(const MtlParameters& parameters) :
    parameters_(convertFromMtl(parameters)) {}

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

float DiffuseMaterial::calculatePdf(
    const Vector3f& normal,
    const Vector3f& inDirection,
    const Vector3f& outDirection
) const {
    const float cosinus = // wi * n
        std::max(EPSILON, getDotProduct(inDirection, normal));

    return cosinus / float(std::numbers::pi);
}

LinearColor DiffuseMaterial::calculateBrdf(
    const Point3f& origin,
    const Vector3f& normal,
    const Vector3f& outDirection,
    const Vector3f& inDirection
) const {
    (void) origin;
    (void) normal;
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
        transformToWorldSpace(localVersor, normal).getNormalized();

    const LinearColor brdf =
        calculateBrdf(origin, normal, outDirection, inDirection);
    const float pdf = localVersor.getZ() / float(std::numbers::pi);

    return MaterialSample {
        .inDirection = inDirection, .brdf = brdf, .pdf = pdf
    };
}
}