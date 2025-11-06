#pragma once

#include <type_traits>
#include "var/var.h"
#include "print.h"

#define varclass(name, body, ...) \
    class Construct_##name __VA_OPT__( : public Construct_##__VA_ARGS__ )  { \
    public: \
        using name = varia::var<Construct_##name>; \
        Construct_##name() = default; \
        virtual ~Construct_##name() = default; \
        Construct_##name(const Construct_##name& other) = default; \
        Construct_##name& operator=(const Construct_##name& other) = default; \
        Construct_##name(Construct_##name&& other) = default; \
        Construct_##name& operator=(Construct_##name&& other) = default;  \
        body \
    }; \
    using name = varia::var<Construct_##name, SharedStorage>;

#define varstruct(name, body, ...) \
    struct Construct_##name __VA_OPT__( : public Construct_##__VA_ARGS__ ) { \
        template<typename> \
        static constexpr auto check_polymorphic_bases = []<typename... Args> { \
            static_assert((!std::is_polymorphic_v<Args> && ...)); \
        }.template operator()<__VA_ARGS__>(); \
        using name = varia::var<Construct_##name>; \
        Construct_##name() = default; \
        ~Construct_##name() = default; \
        Construct_##name(const Construct_##name& other) = default; \
        Construct_##name& operator=(const Construct_##name& other) = default; \
        Construct_##name(Construct_##name&& other) = default; \
        Construct_##name& operator=(Construct_##name&& other) = default;  \
        body \
    }; \
    static_assert(!std::is_polymorphic_v<Construct_##name>); \
    using name = varia::var<Construct_##name, CopiedStorage>;
