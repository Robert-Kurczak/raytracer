#pragma once

#include "Camera/CameraParameters.hpp"
#include "Math/Point.hpp"
#include "Ray/Ray.hpp"

#include <cstdint>

namespace RTC {
class Camera {
private:
    static constexpr float focalLength = 1.0F;
    static constexpr float viewportHeight = 2.0F;

    Point3<float> cameraCenter_ {0, 0, 0};
    Vector2<uint32_t> screenSize_ {0, 0};

    Vector2<float> viewportSize_ {0, 0};
    Vector3<float> viewportU_ {0, 0, 0};
    Vector3<float> viewportV_ {0, 0, 0};
    Point3<float> viewportTopLeft_ {0, 0, 0};

    Vector3<float> pixelDeltaU_ {0, 0, 0};
    Vector3<float> pixelDeltaV_ {0, 0, 0};
    Point3<float> pixelTopLeft_ {0, 0, 0};

    void setupScreenData(const CameraParameters& parameters);
    void setupViewportData();
    void setupPixelData();

    [[nodiscard]] Vector2<float> getRandomPixelOffset() const;

    [[nodiscard]] Ray getRay(
        const Point2<uint32_t>& pixel,
        const Vector2<float>& pixelOffset
    ) const;

public:
    Camera(const CameraParameters& parameters);

    [[nodiscard]] const Vector2<uint32_t>& getSceenSize() const;

    [[nodiscard]] Ray getRay(const Point2<uint32_t>& pixel) const;

    [[nodiscard]] Ray getRandomizedRay(
        const Point2<uint32_t>& pixel
    ) const;
};
}