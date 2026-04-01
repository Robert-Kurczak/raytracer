#pragma once

#include <cstdint>
#include <string_view>

namespace RTC {
enum class LogLevel : uint8_t { Info, Debug, Error };

constexpr std::string_view toString(LogLevel level) {
    switch (level) {
    case LogLevel::Info:
        return "Info";
    case LogLevel::Debug:
        return "Debug";
    case LogLevel::Error:
        return "Error";
    }

    return "Debug";
}

class ILogger {
public:
    ILogger() = default;
    ILogger(const ILogger&) = delete;
    ILogger(ILogger&&) = delete;
    ILogger operator=(const ILogger&) = delete;
    ILogger operator=(ILogger&&) = delete;
    virtual ~ILogger() = default;

    virtual void log(LogLevel level, const std::string_view& text) = 0;
};
}