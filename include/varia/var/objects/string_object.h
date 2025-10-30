#pragma once

#include <format>
#include "object_hierarchy.h"
#include "../../formatting.h"

namespace varia::objects {
    String to_string(const concepts::Formatable auto& value) {
        return String{VARIA_FORMAT_NS::format("{}", value)};
    }

    template<concepts::Arithmetic T>
    T to_arithmetic(const std::string_view str) {
        T value{};
        std::from_chars(str.data(), str.data() + str.size(), value);
        return value;
    }
}
