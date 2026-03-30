#include "Camera.hpp"

#include "CameraParameters.hpp"
#include "Core/Math/Numeric.hpp"
#include "Core/Math/Random.hpp"
#include "Core/Math/Vector.hpp"
#include "Core/Ray/Ray.hpp"

#include <cmath>

namespace RTC {
void Camera::setupViewportData(const CameraParameters& parameters) {
    const double actualAspectRatio =
        double(screenSize_.getX()) / double(screenSize_.getY());

    const double fovRadians =
        degreesToRadians(parameters.fieldOfViewDegrees);

    const double tanTheta = std::tan(fovRadians / 2.0);
    const auto viewportHeight = float(2.0 * focalLength * tanTheta);

    viewportSize_ = {
        float(viewportHeight * actualAspectRatio), viewportHeight
    };

    const Vector3<float> lookDirection =
        parameters.direction.getNormalized();

    const Vector3<float> upDirection =
        parameters.upDirection.getNormalized();

    viewportU_ =
        getCrossProduct(upDirection, -lookDirection).getNormalized();
    viewportV_ =
        getCrossProduct(-lookDirection, viewportU_).getNormalized();

    viewportU_ *= viewportSize_.getX();
    viewportV_ *= -viewportSize_.getY();

    const Point3<float> viewportCenter =
        cameraCenter_ + lookDirection * focalLength;

    viewportTopLeft_ =
        viewportCenter - (viewportU_ / 2.0F) - (viewportV_ / 2.0F);
}

void Camera::setupPixelData() {
    pixelDeltaU_ = viewportU_ / float(screenSize_.getX());
    pixelDeltaV_ = viewportV_ / float(screenSize_.getY());
    pixelTopLeft_ =
        viewportTopLeft_ + (pixelDeltaU_ + pixelDeltaV_) / 2.0F;
}

Vector2<float> Camera::getRandomPixelOffset() const {
    return Point2<float> {
        getRandomNumber<float>() - 0.5F, getRandomNumber<float>() - 0.5F
    };
}

Camera::Camera(const CameraParameters& parameters) :
    cameraCenter_(parameters.position),
    screenSize_({parameters.screenWidth, parameters.screenHeight}) {
    setupViewportData(parameters);
    setupPixelData();
}

const Vector2<uint32_t>& Camera::getSceenSize() const {
    return screenSize_;
}

Ray Camera::getRay(
    const Point2<uint32_t>& pixel,
    const Point2<float>& pixelOffset
) const {
    const Vector3<float> pixelUOffset =
        (float(pixel.getX()) + pixelOffset.getX()) * pixelDeltaU_;

    const Vector3<float> pixelVOffset =
        (float(pixel.getY()) + pixelOffset.getY()) * pixelDeltaV_;

    const Point3<float> pixelCenter =
        pixelTopLeft_ + pixelUOffset + pixelVOffset;

    const Point3<float> target = pixelCenter - cameraCenter_;

    return Ray {cameraCenter_, target};
}

Ray Camera::getRay(const Point2<uint32_t>& pixel) const {
    return getRay(pixel, Point2<float> {0.0F, 0.0F});
}

[[nodiscard]] Ray Camera::getRandomizedRay(
    const Point2<uint32_t>& pixel
) const {
    return getRay(pixel, getRandomPixelOffset());
}
}