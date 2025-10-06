#pragma once
#include "copied_storage.h"
#include "shared_ref_storage.h"

namespace varia::storage {
    template<objects::VarCompatible T>
    using DefaultValueStorage = std::conditional_t<objects::Copied<T>, CopiedStorage<T>, SharedRefStorage<T> >;

    template<typename Derived>
    concept Storage = std::is_base_of_v<BaseStorage<typename Derived::ValueType, Derived>, Derived>;
}
