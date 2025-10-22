#pragma once

#include "var/var.h"
#include "print.h"

#define varclass(name, body, ...) \
    struct Construct_##name __VA_OPT__( : Construct_##__VA_ARGS__ )  { \
        using name = varia::var<Construct_##name>; \
        Construct_##name() = default; \
        virtual ~Construct_##name() = default; \
        Construct_##name(const Construct_##name& other) = default; \
        Construct_##name& operator=(const Construct_##name& other) = default; \
        Construct_##name(Construct_##name&& other) = default; \
        Construct_##name& operator=(Construct_##name&& other) = default;  \
        body \
    }; \
    using name = varia::var<Construct_##name>;
