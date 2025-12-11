#pragma once

#include <type_traits>
#include "var/var.h"
#include "print.h"

namespace varia::objects {
    struct EmptyPolymorphic {
        virtual ~EmptyPolymorphic() = default;
    };

    struct InterfaceTag {
    };

    namespace concepts {
        template<typename T>
        concept Empty = std::is_empty_v<T> || (std::is_polymorphic_v<T> && sizeof(T) == sizeof(EmptyPolymorphic));

        template<typename T>
        concept InterfaceLike = Empty<T> && (!std::is_polymorphic_v<T> || std::is_abstract_v<T>);

        template<typename T>
        concept Interface = std::derived_from<T, InterfaceTag> && InterfaceLike<T>;

        template<typename... Args>
        concept AllInterfaces = (true && ... && objects::concepts::Interface<Args>);
    }

    template<typename...>
    struct IsSingleInheritance {
        static constexpr bool value{true};
    };

    template<typename T1, typename T2, typename... Args>
    struct IsSingleInheritance<T1, T2, Args...> {
        static constexpr bool value{
            objects::concepts::Interface<T2> && (objects::concepts::Interface<Args> && ...)
        };
    };

    template<typename... Args>
    static constexpr bool is_single_inheritance_v = IsSingleInheritance<Args...>::value;
}

#define VARIA_DEFAULT_COPY_MOVE_CONSTRUCTORS(class_name) \
    class_name(const class_name&) = default; \
    class_name& operator=(const class_name&) = default; \
    class_name(class_name&&) = default; \
    class_name& operator=(class_name&&) = default;

#define VARIA_VARINTERFACE(name, body, ...) \
    struct Construct_##name : varia::objects::InterfaceTag __VA_OPT__(, __VA_ARGS__) { \
        static_assert(varia::objects::concepts::AllInterfaces<__VA_ARGS__>, \
            "varia: varinterface can only inherit from other interfaces"); \
        virtual ~Construct_##name() = default; \
        body \
    }; \
    \
    static_assert(varia::objects::concepts::Empty<Construct_##name>, \
        "varia: varinterface cannot contain any data members"); \
    static_assert(varia::objects::concepts::Interface<Construct_##name>, \
        "varia: varinterface virtual member functions must be pure virtual"); \
    using name = Construct_##name;

#define VARIA_VARCLASS(name, body, ...) \
    class Construct_##name __VA_OPT__( : public Construct_##__VA_ARGS__ )  { \
    public: \
        static_assert(varia::objects::is_single_inheritance_v<__VA_OPT__(Construct_##__VA_ARGS__)>, \
            "varia: varclass can only inherit from one varclass and/or extend multiple varinterfaces: "#__VA_ARGS__); \
        \
        using name = varia::var<Construct_##name>; \
        \
        Construct_##name() = default; \
        virtual ~Construct_##name() = default; \
        VARIA_DEFAULT_COPY_MOVE_CONSTRUCTORS(Construct_##name) \
        \
        body \
    }; \
    using name = varia::var<Construct_##name, RefStorage>;

#define VARIA_VARSTRUCT(name, body, ...) \
    struct Construct_##name __VA_OPT__( : public Construct_##__VA_ARGS__ ) { \
        static_assert(varia::objects::is_single_inheritance_v<__VA_OPT__(Construct_##__VA_ARGS__)>, \
            "varia: varstruct cannot inherit from multiple types: "#__VA_ARGS__); \
        \
        template<typename> \
        static constexpr auto check_polymorphic_bases = []<typename... Args> { \
            static_assert((!std::is_polymorphic_v<Args> && ...), \
                "varia: varstruct cannot inherit from a polymorphic type (such as a varclass or varinterface): "#__VA_ARGS__); \
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
    using name = varia::var<Construct_##name, ValueStorage>;

#define VARIA_TEMPLATE_VARCLASS(name, body, ...) \
    class Construct_##name __VA_OPT__( : public Construct_##__VA_ARGS__) { \
    public: \
        static_assert(varia::objects::is_single_inheritance_v<__VA_OPT__(Construct_##__VA_ARGS__)>, \
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
    template<typename... Args> \
    using name = varia::var<Construct_##name<Args...>, RefStorage>;
