#pragma once
#include "object_hierarchy.h"

namespace varia {
    namespace objects {
        struct None : CopiedObject {
            static const std::string& none_string() {
                static std::string str{"none"};
                return str;
            }
        };
    }

    inline constexpr objects::None none{};
}

