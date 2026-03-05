#include "Camera.hpp"

#include "Camera/CameraParameters.hpp"
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

Camera::Camera(const CameraParameters& parameters) {
    setupScreenData(parameters);
    setupViewportData();
    setupPixelData();
}

Ray Camera::getRay(const Point2<uint32_t>& pixel) const {
    const Vector3<float> pixelUOffset =
        float(pixel.getX()) * pixelDeltaU_;

    const Vector3<float> pixelVOffset =
        float(pixel.getY()) * pixelDeltaV_;

    const Point3<float> pixelCenter =
        pixelTopLeft_ + pixelUOffset + pixelVOffset;

    return Ray {cameraCenter_, pixelCenter - cameraCenter_};
}
}