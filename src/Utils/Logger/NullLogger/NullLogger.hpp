#pragma once

#include "Utils/Logger/ILogger.hpp"

namespace RTC {
class NullLogger : public ILogger {
    void log(LogLevel level, const std::string_view& text) override;
};
}