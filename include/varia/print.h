#pragma once
#include <format>
#include "var/var.h"

template<varia::objects::Object T>
struct std::formatter<varia::var<T> > : std::formatter<T> {
    template<typename FormatContext>
    auto format(const varia::var<T>& v, FormatContext& ctx) const {
        return std::formatter<T>::format(get(v), ctx);
    }
};

namespace varia {
    template<typename... Args>
    void print(std::format_string<Args...> fmt, Args&&... args) {
        std::cout << std::format(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void println(std::format_string<Args...> fmt, Args&&... args) {
        std::cout << std::format(fmt, std::forward<Args>(args)...) << '\n';
    }

    template<typename... Args>
    void print(const Var auto& fmt, Args&&... args) {
        std::cout << std::vformat(get(String{fmt}), std::make_format_args(std::forward<Args>(args)...));
    }

    template<typename... Args>
    void println(const Var auto& fmt, Args&&... args) {
        std::cout << std::vformat(get(String{fmt}), std::make_format_args(std::forward<Args>(args)...)) << '\n';
    }

    template<typename T>
    concept Printable = objects::Arithmetic<T> || std::same_as<T, objects::None>;

    void print(const Printable auto& fmt) {
        std::cout << String{fmt};
    }

    void println(const Printable auto& fmt) {
        std::cout << String{fmt} << '\n';
    }
}
