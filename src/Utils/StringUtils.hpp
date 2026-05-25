#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace RTC {
std::vector<std::string> split(
    const std::string& text,
    const char& delimeter
) {
    std::vector<std::string> result;
    std::stringstream textStream {text};
    std::string item;

    while (std::getline(textStream, item, delimeter)) {
        result.push_back(item);
    }

    return result;
}
}