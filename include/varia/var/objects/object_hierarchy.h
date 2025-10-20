#pragma once
#include <cstdint>
#include <string>

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
}

