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

#define VARIA_DEFAULT_COPY_MOVE_CONSTRUCTORS(class_name) \
    class_name(const class_name&) = default; \
    class_name& operator=(const class_name&) = default; \
    class_name(class_name&&) = default; \
    class_name& operator=(class_name&&) = default;

#define VARIA_OBJECT(class_name) Construct_##class_name

#define varclass(name, body, ...) \
    class VARIA_OBJECT(name) __VA_OPT__( : public VARIA_OBJECT(__VA_ARGS__) )  { \
    public: \
        static_assert(varia::detail::is_single_inheritance_v<__VA_OPT__(VARIA_OBJECT(__VA_ARGS__))>, \
            "varia: varclass cannot inherit from multiple types: "#__VA_ARGS__); \
        \
        using name = varia::var<VARIA_OBJECT(name)>; \
        \
        VARIA_OBJECT(name)() = default; \
        virtual ~VARIA_OBJECT(name)() = default; \
        VARIA_DEFAULT_COPY_MOVE_CONSTRUCTORS(VARIA_OBJECT(name)) \
        \
        body \
    }; \
    using name = varia::var<VARIA_OBJECT(name), SharedStorage>;

#define varstruct(name, body, ...) \
    struct VARIA_OBJECT(name) __VA_OPT__( : public VARIA_OBJECT(__VA_ARGS__) ) { \
        static_assert(varia::detail::is_single_inheritance_v<__VA_OPT__(VARIA_OBJECT(__VA_ARGS__))>, \
            "varia: varstruct cannot inherit from multiple types: "#__VA_ARGS__); \
        \
        template<typename> \
        static constexpr auto check_polymorphic_bases = []<typename... Args> { \
            static_assert((!std::is_polymorphic_v<Args> && ...), \
                "varia: varstruct cannot inherit from a polymorphic type (such as a varclass): "#__VA_ARGS__); \
        }.template operator()<__VA_OPT__(VARIA_OBJECT(__VA_ARGS__))>(); \
        \
        using name = varia::var<VARIA_OBJECT(name)>; \
        \
        VARIA_OBJECT(name)() = default; \
        ~VARIA_OBJECT(name)() = default; \
        VARIA_DEFAULT_COPY_MOVE_CONSTRUCTORS(VARIA_OBJECT(name)) \
        \
        body \
    }; \
    static_assert(!std::is_polymorphic_v<VARIA_OBJECT(name)>); \
    \
    using name = varia::var<VARIA_OBJECT(name), CopiedStorage>;

#define template_varclass(name, body, ...) \
    class VARIA_OBJECT(name) __VA_OPT__( : public VARIA_OBJECT(__VA_ARGS__)) { \
    public: \
        static_assert(varia::detail::is_single_inheritance_v<__VA_OPT__(VARIA_OBJECT(__VA_ARGS__))>, \
        "varia: varclass cannot inherit from multiple types: "#__VA_ARGS__); \
        \
        using name = varia::var<VARIA_OBJECT(name)>; \
        \
        VARIA_OBJECT(name)() = default; \
        virtual ~VARIA_OBJECT(name)() = default; \
        VARIA_DEFAULT_COPY_MOVE_CONSTRUCTORS(VARIA_OBJECT(name)) \
        \
        body \
    }; \
    template<typename... Args> \
    using name = varia::var<VARIA_OBJECT(name)<Args...>, varia::SharedStorage>;
