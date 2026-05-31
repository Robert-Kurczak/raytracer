#pragma once

#include "../IProgressIndicator.hpp"

#include <thread>

namespace RTC {
class CoutProgressIndicator : public IProgressIndicator {
private:
    std::atomic<uint32_t> goal_;
    std::atomic<uint32_t> ticks_;
    std::atomic<bool> isRunning_ {false};

    std::jthread thread_;

    static void showProgress(float unitValue);
    static void startWatcherThread(
        const std::stop_token& stopToken,
        std::atomic<bool>& isRunning,
        std::atomic<uint32_t>& ticks,
        std::atomic<uint32_t>& goal
    );

public:
    CoutProgressIndicator();

    void setGoal(uint32_t value) override;
    void tick() override;
};
}