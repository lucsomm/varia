#pragma once
#include <format>
#include <iostream>

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
}
