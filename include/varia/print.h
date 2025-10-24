#pragma once
#include <format>
#include <iostream>
#include "var/var.h"

namespace varia {
    template<typename... Args>
    void print(const objects::Formattable auto& fmt, Args&&... args) {
        const auto& str{objects::to_string(get(fmt))};
        std::cout << std::vformat(std::string_view{str}, std::make_format_args(get(args)...));
    }

    template<typename... Args>
    void println(const objects::Formattable auto& fmt, Args&&... args) {
        const auto& str{objects::to_string(get(fmt))};
        std::cout << std::vformat(std::string_view{str}, std::make_format_args(get(args)...)) << '\n';
    }
}
