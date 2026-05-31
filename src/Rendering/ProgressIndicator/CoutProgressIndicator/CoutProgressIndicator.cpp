#include "CoutProgressIndicator.hpp"

#include "Rendering/ProgressIndicator/CoutProgressIndicator/CoutProgressIndicator.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

namespace RTC {
constexpr uint32_t REFRESH_MILLISECONDS = 1000;

void CoutProgressIndicator::showProgress(float unitValue) {
    unitValue = std::min(unitValue, 1.0F);

    constexpr uint8_t barCharactersLength = 70;

    const auto filledSymbols = uint32_t(barCharactersLength * unitValue);
    const uint32_t emptySymbols = barCharactersLength - filledSymbols;

    const std::string bar = std::format(
        "[{}{}] {:.0f}%{}",
        std::string(filledSymbols, '='),
        std::string(emptySymbols, ' '),
        unitValue * 100.0F,
        unitValue >= 0.99F ? "\n" : "\r"
    );

    std::cout << bar;
    std::cout.flush();
}

void CoutProgressIndicator::startWatcherThread(
    const std::stop_token& stopToken,
    std::atomic<bool>& isRunning,
    std::atomic<uint32_t>& ticks,
    std::atomic<uint32_t>& goal
) {
    while (not stopToken.stop_requested()) {
        while (isRunning.load()) {
            const uint32_t currentTicks = ticks.load();
            const uint32_t currentGoal = goal.load();

            if (currentTicks >= currentGoal) {
                break;
            }

            showProgress(float(currentTicks) / float(currentGoal));

            std::this_thread::sleep_for(
                std::chrono::milliseconds {REFRESH_MILLISECONDS}
            );
        }
    }
}

CoutProgressIndicator::CoutProgressIndicator() :
    thread_(
        startWatcherThread,
        std::ref(isRunning_),
        std::ref(ticks_),
        std::ref(goal_)
    ) {}

void CoutProgressIndicator::setGoal(uint32_t value) {
    ticks_.store(0);
    goal_.store(value);
    isRunning_.store(true);
}

void CoutProgressIndicator::tick() {
    ticks_.fetch_add(1);
}
}