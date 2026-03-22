#pragma once

#include "Geometry/Hittable/IHittable.hpp"

#include <memory>
#include <vector>

namespace RTC {
class IBvhBuilder {
public:
    IBvhBuilder() = default;
    IBvhBuilder(const IBvhBuilder&) = delete;
    IBvhBuilder(IBvhBuilder&&) = delete;
    IBvhBuilder operator=(const IBvhBuilder&) = delete;
    IBvhBuilder operator=(IBvhBuilder&&) = delete;
    virtual ~IBvhBuilder() = default;

    [[nodiscard]] virtual std::unique_ptr<IHittable> build(
        std::vector<std::unique_ptr<IHittable>>&& objects
    ) const = 0;
};
}