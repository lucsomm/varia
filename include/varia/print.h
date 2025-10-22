#pragma once
#include <format>
#include <iostream>
#include "var/var.h"

namespace varia {
    template<typename T>
    concept OStreamable = requires(std::ostream& os, const T& value)
    {
        { os << value } -> std::same_as<std::ostream&>;
    };

    void print(const OStreamable auto& value) {
        std::cout << value;
    }

    void println(const OStreamable auto& value) {
        std::cout << value << '\n';
    }

    template<typename... Args>
    void print(const OStreamable auto& fmt, Args&&... args) {
        const auto str{objects::to_string(get(fmt))};
        std::cout << std::vformat(std::string_view{str},
                                  std::make_format_args(get(args)...));
    }

    template<typename... Args>
    void println(const OStreamable auto& fmt, Args&&... args) {
        const auto str{objects::to_string(get(fmt))};
        std::cout << std::vformat(std::string_view{str},
                                  std::make_format_args(get(args)...)) << '\n';
    }
}
