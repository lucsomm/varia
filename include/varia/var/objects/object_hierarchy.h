#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace varia {
    struct Construct_Value {
    };

    struct Construct_Ref {
    };
}

namespace varia::objects {
    using Bool = bool;
    using Int = int64_t;
    using Float = double;
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
        concept String = std::same_as<std::remove_cvref_t<T>, objects::String>;

        template<typename T>
        concept StringLike = std::constructible_from<objects::String, std::remove_cvref_t<T> >;

        template<typename T>
        concept Pointer = std::is_pointer_v<std::decay_t<T> >;

        template<typename T>
        concept Array = detail::is_array_object_v<std::remove_cvref_t<T> >;

        template<typename T>
        concept Primitive = Arithmetic<T> || StringLike<T> || Pointer<T>;
    }
}

