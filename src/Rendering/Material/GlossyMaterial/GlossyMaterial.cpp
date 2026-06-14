#include "GlossyMaterial.hpp"

#include "Core/Color/Color.hpp"
#include "Core/Math/Random.hpp"
#include "Core/Math/Transformations.hpp"
#include "Core/Math/Vector.hpp"
#include "Geometry/Hittable/HitData.hpp"
#include "Rendering/Material/MaterialSample.hpp"

#include <cmath>
#include <numbers>

namespace RTC {
float GlossyMaterial::getAlphaSquared(float roughness) const {
    return std::max(0.005F, std::powf(roughness, 4));
}

Vector3f GlossyMaterial::createMicrofacetNormal() const {
    const auto uTheta = getRandomNumber<float>();
    const auto uPhi =
        getRandomNumber<float>(0.0F, 2.0F * std::numbers::pi);

    const float cosTheta = std::sqrt(
        (1.0F - uTheta) / ((uTheta * (alphaSquared_ - 1.0F)) + 1.0F)
    );

    const float sinTheta = std::sqrt(1.0F - (cosTheta * cosTheta));

    const float x = sinTheta * std::cos(uPhi);
    const float y = sinTheta * std::sin(uPhi);
    const float z = cosTheta;

    return Vector3f {x, y, z};
}

float GlossyMaterial::getDistributionTerm(float normalsCosine) const {
    const float cosineSquared = normalsCosine * normalsCosine;
    const float denominator_term =
        (((alphaSquared_ - 1.0F) * cosineSquared) + 1.0F);

    const float denominator =
        float(std::numbers::pi) * denominator_term * denominator_term;

    return alphaSquared_ / denominator;
}

float GlossyMaterial::getMaskingShadowingLambda(float cosine) const {
    const float cosineSquared = cosine * cosine;
    const float tangentSquared = (1.0F - cosineSquared) / cosineSquared;

    const float nominator =
        std::sqrt(1.0F + (alphaSquared_ * tangentSquared)) - 1.0F;

    return nominator / 2.0F;
}

float GlossyMaterial::getGeometricTerm(
    float outCosine,
    float inCosine
) const {
    const float outShadowing = getMaskingShadowingLambda(outCosine);
    const float inShadowing = getMaskingShadowingLambda(inCosine);

    return 1.0F / (1.0F + outShadowing + inShadowing);
}

LinearColor GlossyMaterial::getFresnelTerm(
    float microfacetOutCosine
) const {
    return fresnelBaseTerm_ +
           (UNIT_LINEAR_COLOR - fresnelBaseTerm_) *
               float(std::pow(1.0F - microfacetOutCosine, 5));
}

GlossyMaterial::MicrofacetData GlossyMaterial::getCookTorranceTerms(
    const Vector3f& outDirection,
    const Vector3f& inDirection,
    const Vector3f& normal,
    const Vector3f& microfacetNormal
) const {
    const float normalsCosine = // m * n
        std::max(0.0F, getDotProduct(microfacetNormal, normal));

    const float outCosine = // wo * n
        std::max(0.0F, getDotProduct(outDirection, normal));

    const float inCosine = // wi * n
        std::max(0.0F, getDotProduct(inDirection, normal));

    const float microfacetOutCosine = // wo * m
        std::max(0.0F, getDotProduct(microfacetNormal, outDirection));

    const float distribution = getDistributionTerm(normalsCosine);
    const float geometry = getGeometricTerm(outCosine, inCosine);
    const LinearColor fresnel = getFresnelTerm(microfacetOutCosine);

    const LinearColor nominator = distribution * geometry * fresnel;
    const float denominator = 4.0F * outCosine * inCosine;

    const LinearColor brdf = nominator / denominator;

    return MicrofacetData {
        .brdf = brdf,
        .distribution = distribution,
        .normalsCosine = normalsCosine,
        .microfacetOutCosine = microfacetOutCosine
    };
}

GlossyMaterial::GlossyMaterial(const GlossyParameters& parameters) :
    fresnelBaseTerm_(parameters.fresnelBaseTerm),
    alphaSquared_(getAlphaSquared(parameters.roughness)) {}

const LinearColor& GlossyMaterial::getEmission() const {
    return BLACK_LINEAR_COLOR;
};

LinearColor GlossyMaterial::getEmission(
    const Point3f& origin,
    const Vector3f& direction
) const {
    return BLACK_LINEAR_COLOR;
};

LinearColor GlossyMaterial::calculateBrdf(
    const HitData& hitData,
    const Vector3f& outDirection,
    const Vector3f& inDirection
) const {
    const float inCosine = getDotProduct(inDirection, hitData.hitNormal);
    const float outCosine =
        getDotProduct(outDirection, hitData.hitNormal);

    if (inCosine <= 0.0F || outCosine <= 0.0F) {
        return BLACK_LINEAR_COLOR;
    }

    const Vector3f microfacetNormal =
        (outDirection + inDirection).getNormalized();

    const MicrofacetData data = getCookTorranceTerms(

        outDirection, inDirection, hitData.hitNormal, microfacetNormal
    );

    return data.brdf;
};

float GlossyMaterial::calculatePdf(
    const Vector3f& normal,
    const Vector3f& inDirection,
    const Vector3f& outDirection
) const {
    const Vector3f microfacetNormal =
        (outDirection + inDirection).getNormalized();

    const float normalsCosine =
        std::max(0.0F, getDotProduct(microfacetNormal, normal));

    const float microfacetOutCosine =
        std::max(0.0F, getDotProduct(outDirection, microfacetNormal));

    const float distribution = getDistributionTerm(normalsCosine);

    return (distribution * normalsCosine) /
           (4.0F * std::abs(microfacetOutCosine));
}

MaterialSample GlossyMaterial::getSample(
    const HitData& hitData,
    const Vector3f& outDirection
) const {
    const Vector3f microfacetNormal =
        transformToWorldSpace(createMicrofacetNormal(), hitData.hitNormal)
            .getNormalized();

    const Vector3f inDirection =
        (-outDirection).getReflected(microfacetNormal);

    const float inCosine = getDotProduct(inDirection, hitData.hitNormal);
    const float outCosine =
        getDotProduct(outDirection, hitData.hitNormal);

    if (inCosine <= 0.0F || outCosine <= 0.0F) {
        return MaterialSample {
            .inDirection = inDirection,
            .brdf = BLACK_LINEAR_COLOR,
            .pdf = 1.0F
        };
    }

    const MicrofacetData data = getCookTorranceTerms(
        outDirection, inDirection, hitData.hitNormal, microfacetNormal
    );

    const float pdf = (data.distribution * data.normalsCosine) /
                      (4.0F * std::abs(data.microfacetOutCosine));

    return MaterialSample {
        .inDirection = inDirection,
        .brdf = data.brdf,
        .pdf = pdf,
        .scatterType = ScatterType::Glossy
    };
};
}