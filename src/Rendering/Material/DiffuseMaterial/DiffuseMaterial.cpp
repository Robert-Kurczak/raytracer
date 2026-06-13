#include "DiffuseMaterial.hpp"

#include "Core/Color/Color.hpp"
#include "Core/Math/Numeric.hpp"
#include "Core/Math/Sampling.hpp"
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
        .colorTexture = parameters.diffuse,
        .emission = parameters.emission / float(std::numbers::pi)
    };
}

DiffuseMaterial::DiffuseMaterial(const DiffuseParameters& parameters) :
    parameters_(parameters) {}

DiffuseMaterial::DiffuseMaterial(const MtlParameters& parameters) :
    parameters_(convertFromMtl(parameters)) {}

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
    const float cosine = // wi * n
        std::max(EPSILON, getDotProduct(inDirection, normal));

    return cosine / float(std::numbers::pi);
}

LinearColor DiffuseMaterial::calculateBrdf(
    const HitData& hitData,
    const Vector3f& outDirection,
    const Vector3f& inDirection
) const {
    (void) outDirection;
    (void) inDirection;

    const LinearColor color = parameters_.colorTexture->sample(
        hitData.textureCoords.getX(), hitData.textureCoords.getY()
    );

    return color / float(std::numbers::pi);
}

MaterialSample DiffuseMaterial::getSample(
    const HitData& hitData,
    const Vector3f& outDirection
) const {
    const Vector3f localVersor = sampleCosineHemisphere();
    const Vector3f inDirection =
        transformToWorldSpace(localVersor, hitData.hitNormal)
            .getNormalized();

    const LinearColor brdf =
        calculateBrdf(hitData, outDirection, inDirection);
    const float pdf = localVersor.getZ() / float(std::numbers::pi);

    return MaterialSample {
        .inDirection = inDirection,
        .brdf = brdf,
        .pdf = pdf,
        .scatterType = ScatterType::Diffuse
    };
}
}