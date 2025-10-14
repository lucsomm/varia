#pragma once
#include <format>

namespace varia {
    namespace detail {
        inline void vprint(std::FILE* out, const std::string&& fmt, const std::format_args args) {
            const std::string str{std::vformat(fmt, args)};

            if (str.size() == 0) {
                return;
            }

            if (const size_t written{std::fwrite(str.data(), 1, str.size(), out)}; written != str.size()) {
                throw std::system_error(errno, std::generic_category(), "varia::vprint fwrite failed");
            }
        }

        inline void vprintln(std::FILE* out, const std::string&& fmt, const std::format_args args) {
            const std::string str{std::vformat(fmt, args) + '\n'};

            if (str.size() == 0) {
                return;
            }

            if (const size_t written{std::fwrite(str.data(), 1, str.size(), out)}; written != str.size()) {
                throw std::system_error(errno, std::generic_category(), "varia::vprint fwrite failed");
            }
        }
    }

    template<typename... Args>
    void print(const auto& fmt, Args&&... args) {
        detail::vprint(stdout, std::move(get(String{fmt})), std::make_format_args(std::forward<Args>(args)...));
    }

    template<typename... Args>
    void println(const auto& fmt, Args&&... args) {
        detail::vprintln(stdout, std::move(get(String{fmt})), std::make_format_args(std::forward<Args>(args)...));
    }
}
