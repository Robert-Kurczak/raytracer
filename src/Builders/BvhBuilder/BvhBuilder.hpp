#pragma once

#include "Builders/BvhBuilder/IBvhBuilder.hpp"
#include "Geometry/Hittable/IHittable.hpp"

#include <memory>
#include <vector>

namespace RTC {
class BvhBuilder : public IBvhBuilder {
private:
    [[nodiscard]] static bool compareBoxes(
        const std::unique_ptr<IHittable>& left,
        const std::unique_ptr<IHittable>& right,
        uint8_t axis
    );

    [[nodiscard]] uint8_t getLongestAxisIndex(
        const std::vector<std::unique_ptr<IHittable>>& objects,
        uint32_t rangeStart,
        uint32_t rangeEnd
    ) const;

    void sortBoxes(
        std::vector<std::unique_ptr<IHittable>>& objects,
        uint32_t rangeStart,
        uint32_t rangeEnd
    ) const;

    std::unique_ptr<IHittable> createInternalNode(
        std::vector<std::unique_ptr<IHittable>>& objects,
        uint32_t rangeStart,
        uint32_t rangeEnd
    ) const;

    [[nodiscard]] std::unique_ptr<IHittable> buildRecursively(
        std::vector<std::unique_ptr<IHittable>>& objects,
        uint32_t rangeStart,
        uint32_t rangeEnd
    ) const;

public:
    [[nodiscard]] std::unique_ptr<IHittable> build(
        std::vector<std::unique_ptr<IHittable>>&& objects
    ) const override;
};
}