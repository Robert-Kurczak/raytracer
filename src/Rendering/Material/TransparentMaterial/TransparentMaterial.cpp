#include "TransparentMaterial.hpp"

#include "Core/Color/Color.hpp"
#include "Core/Math/Random.hpp"
#include "Core/Math/Vector.hpp"
#include "Rendering/Material/MaterialSample.hpp"
#include "Rendering/Material/TransparentMaterial/TransparentParameters.hpp"

namespace RTC {
MaterialSample TransparentMaterial::createReflection(
    const Vector3f& outDirection,
    const Vector3f& refractionNormal
) const {
    return MaterialSample {
        .inDirection = (-outDirection).getReflected(refractionNormal),
        .brdf = parameters_.transmissionColor,
        .pdf = 1.0F,
        .scatterType = ScatterType::Specular
    };
}

float TransparentMaterial::getFresnelFactor(float incidenceCosine) const {
    const float baseTerm = (1.0F - parameters_.refractionIndex) /
                           (1.0F + parameters_.refractionIndex);

    const float squaredBaseTerm = baseTerm * baseTerm;

    return squaredBaseTerm + ((1.0F - squaredBaseTerm) *
                              float(std::pow(1.0F - incidenceCosine, 5)));
}

TransparentMaterial::TransparentMaterial(
    const TransparentParameters& parameters
) :
    parameters_(parameters),
    inverseRefractionIndex_(1.0F / parameters_.refractionIndex) {}

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
    const HitData& hitData,
    const Vector3f& outDirection,
    const Vector3f& inDirection
) const {
    return BLACK_LINEAR_COLOR;
}

MaterialSample TransparentMaterial::getSample(
    const HitData& hitData,
    const Vector3f& outDirection
) const {
    float incidenceCosine =
        getDotProduct(outDirection, hitData.hitNormal);
    Vector3f refractionNormal = hitData.hitNormal;
    float relativeRefractionIndex = parameters_.refractionIndex;

    const bool fromGlasstoAir = incidenceCosine < 0.0F;

    if (fromGlasstoAir) {
        incidenceCosine = -incidenceCosine;
        refractionNormal = -hitData.hitNormal;
        relativeRefractionIndex = inverseRefractionIndex_;
    }

    const float squaredRelatveRefractionIndex =
        relativeRefractionIndex * relativeRefractionIndex;

    const float incidenceSineSquared =
        std::max(0.0F, 1.0F - (incidenceCosine * incidenceCosine));

    const float refractionSineSquared =
        incidenceSineSquared / squaredRelatveRefractionIndex;

    const float refractionCosine =
        std::sqrt(1.0F - refractionSineSquared);

    const bool totalInternalReflection = refractionSineSquared >= 1.0;

    if (totalInternalReflection) {
        return createReflection(outDirection, refractionNormal);
    }

    const Vector3f refractedDirection =
        (-outDirection / relativeRefractionIndex) +
        ((incidenceCosine / relativeRefractionIndex) - refractionCosine) *
            refractionNormal;

    if (getRandomNumber<float>() <= getFresnelFactor(incidenceCosine)) {
        return createReflection(outDirection, refractionNormal);
    }

    return MaterialSample {
        .inDirection = refractedDirection,
        .brdf =
            parameters_.transmissionColor * squaredRelatveRefractionIndex,
        .pdf = 1.0F,
        .scatterType = ScatterType::Specular
    };
}
}