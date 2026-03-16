#pragma once

#include "../IProgressIndicator.hpp"

namespace RTC {
class CoutProgressIndicator : public IProgressIndicator {
public:
    void showProgress(float unitValue) const override;
};
}