#pragma once
#include "object_hierarchy.h"

namespace varia {
    template<objects::Object T, storage::Storage StorageT>
    class var;

    namespace objects {
        struct None : CopiedObject {
            static std::string& none_string() {
                static std::string str{"none"};
                return str;
            }

            template<Object T, storage::Storage StorageT>
            friend class var;
        };
    }

    inline constexpr objects::None none{};
}

