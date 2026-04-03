#include "SolidBackground.hpp"

namespace RTC {
SolidBackground::SolidBackground(Color<float> color) : color_(color) {}

Color<float> SolidBackground::sample(const Ray& ray) const {
    (void) ray;
    return color_;
};
}