#pragma once
#include <format>

namespace varia::detail {
    inline void vprint(std::FILE* out, const std::string_view fmt, const std::format_args args) {
        const std::string str{std::vformat(fmt, args)};

        if (str.size() == 0) {
            return;
        }

        if (const size_t written{std::fwrite(str.data(), 1, str.size(), out)}; written != str.size()) {
            throw std::system_error(errno, std::generic_category(), "varia::vprint fwrite failed");
        }
    }

    inline void vprintln(std::FILE* out, const std::string_view fmt, const std::format_args args) {
        const std::string str{std::vformat(fmt, args) + '\n'};

        if (str.size() == 0) {
            return;
        }

        if (const size_t written{std::fwrite(str.data(), 1, str.size(), out)}; written != str.size()) {
            throw std::system_error(errno, std::generic_category(), "varia::vprint fwrite failed");
        }
    }
}

namespace varia {
    template<typename... Args>
    void print(const String& str, const Args&&... args) {
        detail::vprint(stdout, get(str), std::make_format_args(std::forward<Args>(args)...));
    }

    template<typename... Args>
    void println(const String& str, const Args&&... args) {
        detail::vprintln(stdout, get(str), std::make_format_args(std::forward<Args>(args)...));
    }
}
