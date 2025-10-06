#pragma once
#include <iostream>

namespace varia {
    struct VariaConfig {
        [[nodiscard]] const VariaConfig& enable_boolalpha() const {
            std::cout << std::boolalpha;
            return *this;
        }
    };
}