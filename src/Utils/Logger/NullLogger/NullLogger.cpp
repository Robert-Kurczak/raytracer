#include "NullLogger.hpp"

namespace RTC {
void NullLogger::log(LogLevel level, const std::string_view& text) {
    (void) level;
    (void) text;
}
}