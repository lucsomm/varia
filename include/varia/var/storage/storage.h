#pragma once
#include <concepts>

namespace varia::concepts {
    template<typename T>
    concept Storage = requires(T t)
    {
        typename T::object_type;
        { t.get() } -> std::convertible_to<typename T::object_type*>;
    };
}
