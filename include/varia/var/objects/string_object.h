#pragma once

#include <format>
#include "object_hierarchy.h"

namespace varia::objects {
    template<typename T>
    concept Formatable = requires(T t)
    {
        { std::format("{}", t) };
    };

    String to_string(const Formatable auto& value) {
        return String{std::format("{}", value)};
    }
}
