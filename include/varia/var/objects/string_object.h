#pragma once

#include <format>
#include "object_hierarchy.h"
#include "../../formatting.h"

namespace varia::objects {
    template<typename T>
    concept Formatable = requires(T t)
    {
        { VARIA_FORMAT_NS::format("{}", t) };
    };

    String to_string(const Formatable auto& value) {
        return String{VARIA_FORMAT_NS::format("{}", value)};
    }

    inline Int to_int(const std::string_view str) {
        Int value{};
        std::from_chars(str.data(), str.data() + str.size(), value);
        return value;
    }

    inline Float to_float(const std::string_view str) {
        Float value{};
        std::from_chars(str.data(), str.data() + str.size(), value);
        return value;
    }
}
