#include "DirectLightEstimator.hpp"

#include "Core/Color/Color.hpp"
#include "Core/Math/Numeric.hpp"
#include "Geometry/Light/ILight.hpp"
#include "Rendering/Material/IMaterial.hpp"
#include "Rendering/Material/MaterialSample.hpp"
#include "World/Scene/Scene.hpp"

namespace RTC {
bool DirectLightEstimator::isInShadow(
    const Point3f& origin,
    const Vector3f& toLight,
    const Scene& scene
) const {
    const Ray shadowRay {origin, toLight};

    const Interval<float> interval {
        EPSILON,       // ray origin is at hit point
        1.0F - EPSILON // ray end (light) is at the end of unnormalized
                       // direction
    };

    return scene.hitAny(shadowRay, interval);
}

LinearColor DirectLightEstimator::getLightRadiance(
    const LightSample& lightSample,
    const Scene& scene,
    const HitData& hitData,
    const Point3f& offsetHitPoint,
    const Vector3f& outDirection
) const {
    if (isInShadow(offsetHitPoint, lightSample.toLight, scene)) {
        return LinearColor::black();
    }

    const LinearColor brdf = hitData.material->calculateBrdf(
        hitData, outDirection, lightSample.inDirection
    );

    const float cosine = std::max(
        0.0F, getDotProduct(hitData.hitNormal, lightSample.inDirection)
    );

    const LinearColor lightRadiance =
        brdf * cosine * lightSample.outLight;

    const float lightPdf = lightSample.pdf;
    const float materialPdf = hitData.material->calculatePdf(
        hitData.hitNormal, lightSample.inDirection, outDirection
    );

    return lightRadiance / (lightPdf + materialPdf);
}

LinearColor DirectLightEstimator::getMaterialRadiance(
    const ILight& sampledLight,
    const MaterialSample& materialSample,
    const Scene& scene,
    const HitData& hitData,
    const Point3f& offsetHitPoint,
    const Vector3f& outDirection
) const {
    constexpr Interval<float> renderInterval {
        EPSILON, Interval<float>::infinity()
    };

    const Ray scatterRay {offsetHitPoint, materialSample.inDirection};

    HitData scatteredHitData;
    const bool hitAnything =
        scene.hitClosest(scatterRay, renderInterval, scatteredHitData);

    const float cosine = std::max(
        0.0F, getDotProduct(hitData.hitNormal, materialSample.inDirection)
    );

    if (not hitAnything) {
        return BLACK_LINEAR_COLOR;
    }

    if (scatteredHitData.light.get() != &sampledLight) {
        return BLACK_LINEAR_COLOR;
    }

    const LinearColor materialEmission =
        scatteredHitData.material->getEmission(
            scatteredHitData.hitPoint, -materialSample.inDirection
        );

    if (materialEmission.isBlack()) {
        return BLACK_LINEAR_COLOR;
    };

    const LinearColor materialRadiance =
        materialSample.brdf * cosine * materialEmission;

    const float materialPdf = materialSample.pdf;
    const float lightPdf = sampledLight.calculatePdf(
        offsetHitPoint, materialSample.inDirection
    );

    return materialRadiance / (materialPdf + lightPdf);
}

LinearColor DirectLightEstimator::getScatteringWeight(
    const LightSample& lightSample,
    const Scene& scene,
    const HitData& hitData,
    const Point3f& offsetHitPoint,
    const Vector3f& outDirection
) const {
    if (isInShadow(offsetHitPoint, lightSample.toLight, scene)) {
        return LinearColor::black();
    }

    const LinearColor brdf = hitData.material->calculateBrdf(
        hitData, outDirection, lightSample.inDirection
    );

    const float cosine = std::max(
        0.0F, getDotProduct(hitData.hitNormal, lightSample.inDirection)
    );

    return brdf * cosine / lightSample.pdf;
}

LinearColor DirectLightEstimator::estimateRadiance(
    const ILight& light,
    const Scene& scene,
    const HitData& hitData,
    const Point3f& offsetHitPoint,
    const Vector3f& outDirection
) const {
    const LightSample lightSample = light.getSample(hitData.hitPoint);

    const MaterialSample materialSample =
        hitData.material->getSample(hitData, outDirection);

    const LinearColor lightRadiance = getLightRadiance(
        lightSample, scene, hitData, offsetHitPoint, outDirection
    );

    const LinearColor materialRadiance = getMaterialRadiance(
        light,
        materialSample,
        scene,
        hitData,
        offsetHitPoint,
        outDirection
    );

    return lightRadiance + materialRadiance;
}
}