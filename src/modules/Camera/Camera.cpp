#include "Camera.hpp"

#include "Camera/CameraParameters.hpp"
#include "Math/Random.hpp"
#include "Math/Vector.hpp"
#include "Ray/Ray.hpp"

namespace RTC {
void Camera::setupScreenData(const CameraParameters& parameters) {
    cameraCenter_ = parameters.center;

    const auto imageHeight =
        int32_t(float(parameters.screenWidth) / parameters.aspectRatio);

    if (imageHeight < 0) {
        screenSize_ = {parameters.screenWidth, 1};
    } else {
        screenSize_ = {parameters.screenWidth, uint32_t(imageHeight)};
    }
}

void Camera::setupViewportData() {
    const float actualAspectRatio =
        float(screenSize_.getX()) / float(screenSize_.getY());

    viewportSize_ = {viewportHeight * actualAspectRatio, viewportHeight};
    viewportU_ = {viewportSize_.getX(), 0.0F, 0.0F};
    viewportV_ = {0.0F, -viewportSize_.getY(), 0.0F};

    const Point3<float> viewportCenter =
        cameraCenter_ - Vector3<float> {0.0F, 0.0F, focalLength};

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

Camera::Camera(const CameraParameters& parameters) {
    setupScreenData(parameters);
    setupViewportData();
    setupPixelData();
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