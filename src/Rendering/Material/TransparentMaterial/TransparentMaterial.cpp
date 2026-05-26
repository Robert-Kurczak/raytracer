#include "TransparentMaterial.hpp"

#include "Core/Color/Color.hpp"
#include "Core/Math/Vector.hpp"
#include "Rendering/Material/MaterialSample.hpp"
#include "Rendering/Material/TransparentMaterial/TransparentParameters.hpp"

#include <numbers>

namespace RTC {
constexpr float AIR_REFRACTION_INDEX = 1.0F;

TransparentParameters TransparentMaterial::convertFromMtl(
    const MtlParameters& parameters
) const {
    return TransparentParameters {
        .refractionIndex = parameters.refractionIndex,
        .emission = parameters.emission / float(std::numbers::pi),
        .transmissionFilter =
            UNIT_LINEAR_COLOR - parameters.transmisionFilter
    };
}

TransparentMaterial::TransparentMaterial(
    TransparentParameters parameters
) :
    parameters_(parameters) {}

TransparentMaterial::TransparentMaterial(MtlParameters parameters) :
    parameters_(convertFromMtl(parameters)) {}

const LinearColor& TransparentMaterial::getBaseColor() const {
    return BLACK_LINEAR_COLOR;
}

const LinearColor& TransparentMaterial::getEmission() const {
    return parameters_.emission;
}

LinearColor TransparentMaterial::getEmission(
    const Point3f& origin,
    const Vector3f& direction
) const {
    return parameters_.emission;
}

float TransparentMaterial::calculatePdf(
    const Vector3f& normal,
    const Vector3f& inDirection,
    const Vector3f& outDirection
) const {
    return 1.0F;
}

LinearColor TransparentMaterial::calculateBrdf(
    const Point3f& origin,
    const Vector3f& normal,
    const Vector3f& outDirection,
    const Vector3f& inDirection
) const {
    return BLACK_LINEAR_COLOR;
}

MaterialSample TransparentMaterial::getSample(
    const Point3f& origin,
    const Vector3f& normal,
    const Vector3f& outDirection
) const {
    float incidenceCosine = getDotProduct(outDirection, normal);
    Vector3f refractionNormal = normal;
    float relativeRefractionIndex = parameters_.refractionIndex;

    const bool fromGlasstoAir = incidenceCosine < 0.0F;

    if (fromGlasstoAir) {
        incidenceCosine = -incidenceCosine;
        refractionNormal = -normal;
        relativeRefractionIndex =
            AIR_REFRACTION_INDEX / parameters_.refractionIndex;
    }

    const float incidenceSineSquared =
        std::max(0.0F, 1.0F - (incidenceCosine * incidenceCosine));

    const float refractionSineSquared =
        incidenceSineSquared /
        (relativeRefractionIndex * relativeRefractionIndex);

    const float refractionCosine =
        std::sqrt(1.0F - refractionSineSquared);

    if (refractionSineSquared >= 1.0F) {
        return MaterialSample {
            .inDirection = (-outDirection).getReflected(refractionNormal),
            .brdf = parameters_.transmissionFilter,
            .pdf = 1.0F,
            .scatterType = ScatterType::Specular
        };
    }

    const Vector3f refractedDirection =
        (-outDirection / relativeRefractionIndex) +
        ((incidenceCosine / relativeRefractionIndex) - refractionCosine) *
            refractionNormal;

    return MaterialSample {
        .inDirection = refractedDirection,
        .brdf = parameters_.transmissionFilter * relativeRefractionIndex *
                relativeRefractionIndex,
        .pdf = 1.0F,
        .scatterType = ScatterType::Specular
    };
}
}