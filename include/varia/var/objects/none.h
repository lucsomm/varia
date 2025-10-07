#pragma once
#include "object_hierarchy.h"

namespace varia {
    namespace objects {
        struct None : CopiedObject {
        };
    }

    inline constexpr objects::None none{};
}

