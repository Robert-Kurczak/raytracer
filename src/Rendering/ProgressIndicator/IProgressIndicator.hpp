#pragma once

#include <cstdint>
namespace RTC {
class IProgressIndicator {
public:
    IProgressIndicator() = default;
    IProgressIndicator(const IProgressIndicator&) = delete;
    IProgressIndicator(IProgressIndicator&&) = delete;
    IProgressIndicator operator=(const IProgressIndicator&) = delete;
    IProgressIndicator operator=(IProgressIndicator&&) = delete;
    virtual ~IProgressIndicator() = default;

    virtual void setGoal(uint32_t value) = 0;
    virtual void tick() = 0;
};
}