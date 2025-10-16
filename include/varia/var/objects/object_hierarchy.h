#pragma once
#include <concepts>
#include <cstdint>
#include <string>

namespace varia::objects {
    struct StandardObject {
    };

    struct VarIncompatibleObject {
    };

    struct CopiedObject {
    };

    struct ImmutableRefObject {
    };

    struct ReferencedObject {
    };

    // Copied
    using Int = int64_t;
    using Float = double;
    class Bool;
    class Num;

    // ImmutableRef
    using String = std::string;

    template<typename T>
    concept Arithmetic = std::is_arithmetic_v<T>;

    template<typename T>
    concept ArithmeticNotBool = std::is_arithmetic_v<T> && !std::same_as<T, bool>;

    template<typename T>
    concept ArithmeticLikeNotBool = Arithmetic<T> || std::same_as<T, Num>;

    template<typename T>
    concept ArithmeticLike = ArithmeticLikeNotBool<T> || std::same_as<T, Bool>;

    template<typename T>
    concept Fundamental = std::is_fundamental_v<T>;

    template<typename T>
    concept Object = !std::same_as<void, T>;

    template<typename T>
    concept VarIncompatible = !Object<T>;

    template<typename T>
    concept Copied = std::derived_from<T, CopiedObject> || std::is_fundamental_v<T>;

    template<typename T>
    concept ImmutableRef = std::derived_from<T, ImmutableRefObject> || std::same_as<String, T>;

    template<typename T>
    concept Referenced = std::derived_from<T, ReferencedObject> || ImmutableRef<T>;
}
