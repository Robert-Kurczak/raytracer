#include "GlossyMaterial.hpp"

#include "Core/Color/Color.hpp"
#include "Core/Math/Random.hpp"
#include "Core/Math/Transformations.hpp"
#include "Core/Math/Vector.hpp"
#include "Rendering/Material/MaterialSample.hpp"

#include <cmath>
#include <numbers>

namespace RTC {
GlossyParameters GlossyMaterial::convertFromMtl(
    const MtlParameters& parameters
) const {
    return GlossyParameters {
        .roughness = std::sqrt(2.0F / (parameters.shininess + 2.0F)),
        .fresnelBaseTerm =
            LinearColor {.red = 0.80F, .green = 0.80F, .blue = 0.80F}
    };
}

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

float GlossyMaterial::getDistributionTerm(float normalsCosinus) const {
    const float cosinusSquared = normalsCosinus * normalsCosinus;
    const float denominator_term =
        (((alphaSquared_ - 1.0F) * cosinusSquared) + 1.0F);

    const float denominator =
        float(std::numbers::pi) * denominator_term * denominator_term;

    return alphaSquared_ / denominator;
}

float GlossyMaterial::getMaskingShadowingLambda(float cosinus) const {
    const float cosinusSquared = cosinus * cosinus;
    const float tangensSquared = (1.0F - cosinusSquared) / cosinusSquared;

    const float nominator =
        std::sqrt(1.0F + (alphaSquared_ * tangensSquared)) - 1.0F;

    return nominator / 2.0F;
}

float GlossyMaterial::getGeometricTerm(
    float outCosinus,
    float inCosinus
) const {
    const float outShadowing = getMaskingShadowingLambda(outCosinus);
    const float inShadowing = getMaskingShadowingLambda(inCosinus);

    return 1.0F / (1.0F + outShadowing + inShadowing);
}

LinearColor GlossyMaterial::getFresnelTerm(
    float microfacetOutCosinus
) const {
    return fresnelBaseTerm_ +
           (UNIT_LINEAR_COLOR - fresnelBaseTerm_) *
               float(std::pow(1.0F - microfacetOutCosinus, 5));
}

GlossyMaterial::MicrofacetData GlossyMaterial::getCookTorranceTerms(
    const Vector3f& outDirection,
    const Vector3f& inDirection,
    const Vector3f& normal,
    const Vector3f& microfacetNormal
) const {
    const float normalsCosinus = // m * n
        std::max(0.0F, getDotProduct(microfacetNormal, normal));

    const float outCosinus = // wo * n
        std::max(0.0F, getDotProduct(outDirection, normal));

    const float inCosinus = // wi * n
        std::max(0.0F, getDotProduct(inDirection, normal));

    const float microfacetOutCosinus = // wo * m
        std::max(0.0F, getDotProduct(microfacetNormal, outDirection));

    const float distribution = getDistributionTerm(normalsCosinus);
    const float geometry = getGeometricTerm(outCosinus, inCosinus);
    const LinearColor fresnel = getFresnelTerm(microfacetOutCosinus);

    const LinearColor nominator = distribution * geometry * fresnel;
    const float denominator = 4.0F * outCosinus * inCosinus;

    const LinearColor brdf = nominator / denominator;

    return MicrofacetData {
        .brdf = brdf,
        .distribution = distribution,
        .normalsCosinus = normalsCosinus,
        .microfacetOutCosinus = microfacetOutCosinus
    };
}

GlossyMaterial::GlossyMaterial(const GlossyParameters& parameters) :
    fresnelBaseTerm_(parameters.fresnelBaseTerm),
    alphaSquared_(getAlphaSquared(parameters.roughness)) {}

GlossyMaterial::GlossyMaterial(const MtlParameters& parameters) :
    GlossyMaterial(convertFromMtl(parameters)) {}

const LinearColor& GlossyMaterial::getBaseColor() const {
    return BLACK_LINEAR_COLOR;
}

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
    const Point3f& origin,
    const Vector3f& normal,
    const Vector3f& outDirection,
    const Vector3f& inDirection
) const {
    const float inCosinus = getDotProduct(inDirection, normal);
    const float outCosinus = getDotProduct(outDirection, normal);

    if (inCosinus <= 0.0F || outCosinus <= 0.0F) {
        return BLACK_LINEAR_COLOR;
    }

    const Vector3f microfacetNormal =
        (outDirection + inDirection).getNormalized();

    const MicrofacetData data = getCookTorranceTerms(
        outDirection, inDirection, normal, microfacetNormal
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

    const float normalsCosinus =
        std::max(0.0F, getDotProduct(microfacetNormal, normal));

    const float microfacetOutCosinus =
        std::max(0.0F, getDotProduct(outDirection, microfacetNormal));

    const float distribution = getDistributionTerm(normalsCosinus);

    return (distribution * normalsCosinus) /
           (4.0F * std::abs(microfacetOutCosinus));
}

MaterialSample GlossyMaterial::getSample(
    const Point3f& origin,
    const Vector3f& normal,
    const Vector3f& outDirection
) const {
    const Vector3f microfacetNormal =
        transformToWorldSpace(createMicrofacetNormal(), normal)
            .getNormalized();

    const Vector3f inDirection =
        (-outDirection).getReflected(microfacetNormal);

    const float inCosinus = getDotProduct(inDirection, normal);
    const float outCosinus = getDotProduct(outDirection, normal);

    if (inCosinus <= 0.0F || outCosinus <= 0.0F) {
        return MaterialSample {
            .inDirection = inDirection,
            .brdf = BLACK_LINEAR_COLOR,
            .pdf = 1.0F
        };
    }

    const MicrofacetData data = getCookTorranceTerms(
        outDirection, inDirection, normal, microfacetNormal
    );

    const float pdf = (data.distribution * data.normalsCosinus) /
                      (4.0F * std::abs(data.microfacetOutCosinus));

    return MaterialSample {
        .inDirection = inDirection,
        .brdf = data.brdf,
        .pdf = pdf,
        .scatterType = ScatterType::Glossy
    };
};
}