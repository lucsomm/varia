#pragma once
#include <concepts>
#include <cstdint>
#include <string>

namespace varia::objects {
    struct VarIncompatibleObject {
    };

    struct CopiedObject {
    };

    struct ImmutableRefObject {
    };

    struct ReferencedObject {
    };

    // Copied
    class Bool;
    using Int = int64_t;
    using Float = double;
    using Num = double;

    // ImmutableRef
    using String = std::string;

    template<typename T>
    concept Arithmetic = std::is_arithmetic_v<T>;

    template<typename T>
    concept ArithmeticNotBool = std::is_arithmetic_v<T> && !std::same_as<Bool, T>;

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
