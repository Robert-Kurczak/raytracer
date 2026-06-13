#include "StandardMaterial.hpp"

#include "Core/Math/Random.hpp"

namespace RTC {
MaterialSample StandardMaterial::getGlossySample(
    const HitData& hitData,
    const Vector3f& outDirection
) const {
    MaterialSample glossySample =
        glossyMaterial_->getSample(hitData, outDirection);

    return glossySample;
}

MaterialSample StandardMaterial::getDiffuseSample(
    const HitData& hitData,
    const Vector3f& outDirection
) const {
    MaterialSample diffuseSample =
        diffuseMaterial_->getSample(hitData, outDirection);

    return diffuseSample;
}

StandardMaterial::StandardMaterial(
    std::shared_ptr<IMaterial> diffuseMaterial,
    std::shared_ptr<IMaterial> glossyMaterial,
    float glossyBlendFactor
) :
    diffuseMaterial_(std::move(diffuseMaterial)),
    glossyMaterial_(std::move(glossyMaterial)),
    glossyBlendFactor_(glossyBlendFactor),
    diffuseBlendFactor_(1.0F - glossyBlendFactor_) {}

const LinearColor& StandardMaterial::getEmission() const {
    return diffuseMaterial_->getEmission();
}

LinearColor StandardMaterial::getEmission(
    const Point3f& origin,
    const Vector3f& direction
) const {
    return diffuseMaterial_->getEmission(origin, direction);
}

LinearColor StandardMaterial::calculateBrdf(
    const HitData& hitData,
    const Vector3f& outDirection,
    const Vector3f& inDirection
) const {
    const LinearColor diffuseBrdf = diffuseMaterial_->calculateBrdf(
        hitData, outDirection, inDirection
    );

    const LinearColor glossyBrdf = glossyMaterial_->calculateBrdf(
        hitData, outDirection, inDirection
    );

    return diffuseBlendFactor_ * diffuseBrdf +
           glossyBlendFactor_ * glossyBrdf;
}

float StandardMaterial::calculatePdf(
    const Vector3f& normal,
    const Vector3f& inDirection,
    const Vector3f& outDirection
) const {
    const float diffusePdf =
        diffuseMaterial_->calculatePdf(normal, inDirection, outDirection);
    const float glossyPdf =
        glossyMaterial_->calculatePdf(normal, inDirection, outDirection);

    return (diffuseBlendFactor_ * diffusePdf) +
           (glossyBlendFactor_ * glossyPdf);
}

MaterialSample StandardMaterial::getSample(
    const HitData& hitData,
    const Vector3f& outDirection
) const {
    const auto randomFactor = getRandomNumber<float>();

    if (randomFactor <= glossyBlendFactor_) {
        return getGlossySample(hitData, outDirection);
    }

    return getDiffuseSample(hitData, outDirection);
}
}