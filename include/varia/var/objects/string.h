#pragma once

#include <format>
#include "object_hierarchy.h"

namespace varia::objects {
    template<typename T>
    concept Formattable = requires(T t)
    {
        { std::format("{}", t) };
    };

    String to_string(const Formattable auto& value) {
        return String{std::format("{}", value)};
    }
}
