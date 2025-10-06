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
    using Bool = bool;
    using Int = int64_t;
    using Float = double;
    using Num = double;

    // ImmutableRef
    using String = std::string;

    template<typename T>
    concept Arithmetic = std::is_arithmetic_v<T>;

    template<typename T>
    concept Fundamental = std::is_fundamental_v<T>;

    template<typename T>
    concept Object = !std::same_as<void, T>;

    template<typename T>
    concept VarIncompatible = !Object<T>;

    template<typename T>
    concept Copied = std::is_base_of_v<CopiedObject, T> || std::is_fundamental_v<T>;

    template<typename T>
    concept ImmutableRef = std::is_base_of_v<ImmutableRefObject, T> || std::same_as<String, T>;

    template<typename T>
    concept Referenced = std::is_base_of_v<ReferencedObject, T> || ImmutableRef<T>;
}
