#pragma once
#include <charconv>
#include <limits>
#include "object_hierarchy.h"

namespace varia::objects::detail {
    String to_string(const bool value) {
        return String{
            value ? "true" : "false"
        };
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

    inline Int to_int(const std::string_view str) {
        Int value{};
        std::from_chars(str.data(), str.data() + str.size(), value);
        return value;
    }

    inline Num to_num(const std::string_view str) {
        Num value{};
        std::from_chars(str.data(), str.data() + str.size(), value);
        return value;
    }
}
