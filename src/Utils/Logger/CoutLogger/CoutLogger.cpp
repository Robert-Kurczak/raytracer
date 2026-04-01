#include "CoutLogger.hpp"

#include "Utils/Logger/ILogger.hpp"

#include <iostream>
#include <string_view>

namespace RTC {
constexpr std::string_view colorReset = "\033[0m";

constexpr std::string_view getLevelColor(LogLevel level) {
    constexpr auto white = "\033[35m";
    constexpr auto yellow = "\033[33m";
    constexpr auto red = "\033[31m";

    switch (level) {
    case LogLevel::Info:
        return white;
    case LogLevel::Debug:
        return yellow;
    case LogLevel::Error:
        return red;
    }

    return colorReset;
}

void CoutLogger::log(LogLevel level, const std::string_view& text) {
    std::cout << getLevelColor(level);
    std::cout << "[" << toString(level) << "] ";
    std::cout << text << colorReset << "\n";
}
}