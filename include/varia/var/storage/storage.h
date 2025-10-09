#pragma once
#include "copied_storage.h"
#include "immutable_ref_storage.h"
#include "shared_ref_storage.h"

namespace varia::storage {
    template<objects::Object T>
    using DefaultRefStorage = std::conditional_t<objects::ImmutableRef<T>, ImmutableRefStorage<T>,
        SharedRefStorage<T> >;

    template<objects::Object T>
    using DefaultStorage = std::conditional_t<objects::Copied<T>, CopiedStorage<T>, DefaultRefStorage<T> >;

    template<typename Derived>
    concept Storage = std::derived_from<Derived, BaseStorage<typename Derived::ValueType, Derived> >;
}
