#include "GlossyMaterial.hpp"

#include "Core/Color/Color.hpp"
#include "Core/Math/Random.hpp"
#include "Core/Math/Vector.hpp"
#include "Geometry/Material/MaterialSample.hpp"

#include <numbers>

namespace RTC {
GlossyParameters GlossyMaterial::convertFromMtl(
    const MtlParameters& parameters
) const {
    return GlossyParameters {
        .roughness = std::sqrt(2.0F / (parameters.shininess + 2.0F))
    };
}

Vector3f GlossyMaterial::createMicrofacetNormal() const {
    const auto uTheta = getRandomNumber<float>();
    const auto uPhi = getRandomNumber<float>(0, 2 * std::numbers::pi);

    const float cosTheta =
        (1 - uTheta) / ((uTheta * (alphaSquared_ - 1)) + 1);

    const float sinTheta = std::sqrt(1 - (cosTheta * cosTheta));

    const float x = sinTheta * std::cos(uPhi);
    const float y = sinTheta * std::sin(uPhi);
    const float z = cosTheta;

    return Vector3f {x, y, z};
}

// TODO move to Vector or other common place
Vector3f GlossyMaterial::transformToWorldSpace(
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

    return globalVersor;
}

float GlossyMaterial::getDistributionTerm(
    const Vector3f& microfacetNormal,
    const Vector3f& macrosurfaceNormal
) const {
    const float cosinus =
        getDotProduct(microfacetNormal, macrosurfaceNormal);

    if (cosinus <= 0.0F) {
        return 0.0F;
    }

    const float denominator = (((alphaSquared_ - 1) * cosinus) + 1);

    return alphaSquared_ / (denominator * denominator);
}

float GlossyMaterial::getGeometricTerm(
    const Vector3f& outDirection,
    const Vector3f& inDirection,
    const Vector3f& macrosurfaceNormal
) const {
    static const auto lambda = [&](const Vector3f& direction) {
        const float cosinus =
            getDotProduct(direction, macrosurfaceNormal);

        if (cosinus <= 0.0F) {
            return 0.0F;
        }

        const float cosinusSquared = cosinus * cosinus;
        const float tangensSquared =
            (1.0F - cosinusSquared) / cosinusSquared;

        const float nominator =
            std::sqrt(1.0F + (alphaSquared_ * tangensSquared)) - 1.0F;

        return nominator / 2.0F;
    };

    return 1.0F / (1.0F + lambda(outDirection) + lambda(inDirection));
}

LinearColor GlossyMaterial::getFresnelTerm(
    const Vector3f& outDirection,
    const Vector3f& microfacetNormal
) const {
    constexpr LinearColor baseTerm {
        .red = 0.04F, .green = 0.04F, .blue = 0.04F
    };

    constexpr LinearColor unitColor {
        .red = 1.0F, .green = 1.0F, .blue = 1.0F
    };

    const float cosinus = getDotProduct(outDirection, microfacetNormal);

    return baseTerm +
           (unitColor - baseTerm) * float(std::pow(1.0F - cosinus, 5));
}

GlossyMaterial::GlossyMaterial(const MtlParameters& parameters) :
    parameters_(convertFromMtl(parameters)),
    alphaSquared_(std::powf(parameters_.roughness, 4)) {}

const LinearColor& GlossyMaterial::getBaseColor() const {
    static auto color = LinearColor::black();
    return color;
}

const LinearColor& GlossyMaterial::getEmission() const {
    static auto color = LinearColor::black();
    return color;
};

LinearColor GlossyMaterial::getEmission(
    const Point3f& origin,
    const Vector3f& direction
) const {
    static auto color = LinearColor::black();
    return color;
};

LinearColor GlossyMaterial::calculateBrdf(
    const Point3f& origin,
    const Vector3f& outDirection,
    const Vector3f& inDirection
) const {
    const Vector3f microfacetNormal =
        (inDirection + outDirection).getNormalized();

    //
};

MaterialSample GlossyMaterial::getSample(
    const Point3f& origin,
    const Vector3f& normal,
    const Vector3f& outDirection
) const {
    // TODO TBN to global
    const Vector3f microfacetNormal =
        transformToWorldSpace(createMicrofacetNormal(), normal);
    const Vector3f inDirection =
        outDirection.getReflected(microfacetNormal);

    // TODO compute cosinuses wi * n and wo * n here and pass them

    const float distribution =
        getDistributionTerm(microfacetNormal, normal);

    const float geometry =
        getGeometricTerm(outDirection, inDirection, normal);

    const LinearColor fresnel =
        getFresnelTerm(outDirection, microfacetNormal);

    const float outCosinus = getDotProduct(outDirection, normal);
    const float inCosinus = getDotProduct(inDirection, normal);
    const float microfacetOutCosinus =
        getDotProduct(microfacetNormal, outDirection);
    const float microfacetMacrosurfaceCosinus =
        getDotProduct(microfacetNormal, normal);

    const LinearColor nominator = distribution * geometry * fresnel;
    const float denominator = 4.0F * outCosinus * inCosinus;

    const LinearColor brdf = nominator / denominator;
    const float pdf = (distribution * microfacetMacrosurfaceCosinus) /
                      (4.0F * microfacetOutCosinus);

    return MaterialSample {
        .inDirection = inDirection, .brdf = brdf, .pdf = pdf
    };
};
}