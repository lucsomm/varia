#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace varia::objects {
    struct CopiedObject {
    };

    struct ImmutableSharedObject {
    };

    struct SharedObject {
    };

    using Bool = bool;
    using Int = int64_t;
    using Float = double;
    using Num = double;
    using String = std::string;
    template<typename T, typename Alloc = std::allocator<T> >
    using Array = std::vector<T, Alloc>;
    template<typename K, typename V>
    using Map = std::unordered_map<K, V>;

    namespace detail {
        template<typename>
        struct is_array_object : std::false_type {
        };

        template<typename T, typename Alloc>
        struct is_array_object<Array<T, Alloc> > : std::true_type {
        };

        template<typename T>
        inline constexpr bool is_array_object_v = is_array_object<T>::value;
    }

    namespace concepts {
        template<typename T>
        concept Arithmetic = std::is_arithmetic_v<std::remove_cvref_t<T> >;

        template<typename T>
        concept Array = detail::is_array_object_v<std::remove_cvref_t<T> >;
    }
}

