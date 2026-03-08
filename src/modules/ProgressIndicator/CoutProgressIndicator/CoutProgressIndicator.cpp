#include "CoutProgressIndicator.hpp"

#include "ProgressIndicator/CoutProgressIndicator/CoutProgressIndicator.hpp"

#include <algorithm>
#include <cstdint>
#include <iostream>

namespace RTC {
void CoutProgressIndicator::showProgress(float unitValue) const {
    unitValue = std::min(unitValue, 1.0F);

    constexpr uint8_t barCharactersLength = 70;
    constexpr char emptyBarSymbol = ' ';
    constexpr char filledBarSymbol = '=';

    std::cout << "[";

    const auto filledSymbols = uint32_t(barCharactersLength * unitValue);
    const uint32_t emptySymbols = barCharactersLength - filledSymbols;

    for (uint32_t i = 0; i < filledSymbols; i++) {
        std::cout << filledBarSymbol;
    }

    for (uint32_t i = 0; i < emptySymbols; i++) {
        std::cout << emptyBarSymbol;
    }

    std::cout << "] " << int(unitValue * 100.0F) << "%\r";
    std::cout.flush();
}
}