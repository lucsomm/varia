#pragma once

#if __has_include(<fmt/core.h>)
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

#define VARIA_FORMAT_NS fmt
#else
#include <format>

#define VARIA_FORMAT_NS std
#endif

namespace varia::objects::concepts {
    template<typename T>
    concept Formatable = requires(T t)
    {
        { VARIA_FORMAT_NS::format("{}", t) };
    };
}
