#include "SolidBackground.hpp"

namespace RTC {
SolidBackground::SolidBackground(LinearColor color) : color_(color) {}

LinearColor SolidBackground::sample(const Ray& ray) const {
    (void) ray;
    return color_;
};
}