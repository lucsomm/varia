#pragma once
#include <charconv>
#include <limits>

#include "object_hierarchy.h"

namespace varia::objects {
    inline std::string_view to_string(const char* str) noexcept {
        return std::string_view{str};
    }

    inline const String& to_string(const String& str) noexcept {
        return str;
    }

    inline String&& to_string(String&& str) noexcept {
        return std::move(str);
    }

    inline String to_string(const bool cond) {
        return String{cond ? "true" : "false"};
    }

    String to_string(const std::integral auto value) {
        return std::to_string(value);
    }

    String to_string(const std::floating_point auto value) {
        constexpr size_t buffer_size = std::numeric_limits<std::decay_t<decltype(value)> >::max_digits10;
        std::string str(buffer_size, '\0');
        auto [ptr, ec] = std::to_chars(str.data(), str.data() + str.size(),
                                       value, std::chars_format::general);
        str.resize(ptr - str.data());
        return str;
    }
}
