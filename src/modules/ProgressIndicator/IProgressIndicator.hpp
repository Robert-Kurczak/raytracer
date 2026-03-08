#pragma once

namespace RTC {
class IProgressIndicator {
public:
    IProgressIndicator() = default;
    IProgressIndicator(const IProgressIndicator&) = delete;
    IProgressIndicator(IProgressIndicator&&) = delete;
    IProgressIndicator operator=(const IProgressIndicator&) = delete;
    IProgressIndicator operator=(IProgressIndicator&&) = delete;
    virtual ~IProgressIndicator() = default;

    virtual void showProgress(float unitValue) const = 0;
};
}