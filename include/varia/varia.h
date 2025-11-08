#pragma once

#include <type_traits>
#include "var/var.h"
#include "print.h"

namespace varia::detail {
    template<typename... Args>
    struct ArgsCount {
        static constexpr size_t value{sizeof...(Args)};
    };

    template<typename... Args>
    static constexpr auto args_count_v = ArgsCount<Args...>::value;

    template<typename... Args>
    struct IsSingleInheritance {
        static constexpr bool value{args_count_v<Args...> <= 1};
    };

    template<typename... Args>
    static constexpr bool is_single_inheritance_v = IsSingleInheritance<Args...>::value;
}

#define VARIA_DEFAULT_COPY_MOVE_CONSTRUCTORS(name) \
    name(const name& other) = default; \
    name& operator=(const name& other) = default; \
    name(name&& other) = default; \
    name& operator=(name&& other) = default;

#define varclass(name, body, ...) \
    class Construct_##name __VA_OPT__( : public Construct_##__VA_ARGS__ )  { \
    public: \
        static_assert(varia::detail::is_single_inheritance_v<__VA_OPT__(Construct_##__VA_ARGS__)>, \
            "varia: varclass cannot inherit from multiple types: "#__VA_ARGS__); \
        \
        using name = varia::var<Construct_##name>; \
        \
        Construct_##name() = default; \
        virtual ~Construct_##name() = default; \
        VARIA_DEFAULT_COPY_MOVE_CONSTRUCTORS(Construct_##name) \
        \
        body \
    }; \
    using name = varia::var<Construct_##name, SharedStorage>;

#define varstruct(name, body, ...) \
    struct Construct_##name __VA_OPT__( : public Construct_##__VA_ARGS__ ) { \
        static_assert(varia::detail::is_single_inheritance_v<__VA_OPT__(Construct_##__VA_ARGS__)>, \
            "varia: varstruct cannot inherit from multiple types: "#__VA_ARGS__); \
        \
        template<typename> \
        static constexpr auto check_polymorphic_bases = []<typename... Args> { \
            static_assert((!std::is_polymorphic_v<Args> && ...), \
                "varia: varstruct cannot inherit from a polymorphic type (such as a varclass): "#__VA_ARGS__); \
        }.template operator()<__VA_OPT__(Construct_##__VA_ARGS__)>(); \
        \
        using name = varia::var<Construct_##name>; \
        \
        Construct_##name() = default; \
        ~Construct_##name() = default; \
        VARIA_DEFAULT_COPY_MOVE_CONSTRUCTORS(Construct_##name) \
        \
        body \
    }; \
    static_assert(!std::is_polymorphic_v<Construct_##name>); \
    \
    using name = varia::var<Construct_##name, CopiedStorage>;
