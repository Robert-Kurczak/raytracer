#pragma once

#include "Utils/Logger/ILogger.hpp"

namespace RTC {
class CoutLogger : public ILogger {
public:
    void log(LogLevel level, const std::string_view& text) override;
};
}