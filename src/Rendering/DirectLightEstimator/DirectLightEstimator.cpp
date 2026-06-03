#include "DirectLightEstimator.hpp"

#include "Core/Math/Numeric.hpp"
#include "Rendering/Material/IMaterial.hpp"
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
LinearColor DirectLightEstimator::estimate(
    const ILight& light,
    const Scene& scene,
    const HitData& hitData,
    const Point3f& offsetHitPoint,
    const Vector3f& outDirection
) const {
    const LightSample lightSample = light.getSample(hitData.hitPoint);

    if (isInShadow(offsetHitPoint, lightSample.toLight, scene)) {
        return LinearColor::black();
    }

    const LinearColor brdf = hitData.material->calculateBrdf(
        hitData.hitPoint,
        hitData.hitNormal,
        outDirection,
        lightSample.inDirection
    );

    const float cosine = std::max(
        0.0F, getDotProduct(hitData.hitNormal, lightSample.inDirection)
    );

    return brdf * lightSample.outLight * cosine / lightSample.pdf;
}
}