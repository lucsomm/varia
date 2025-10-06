#pragma once
#include "../objects/object_hierarchy.h"

namespace varia::storage {
    template<objects::VarCompatible T, typename Derived>
    struct BaseStorage {
    private:
        BaseStorage() = default;

    public:
        using ValueType = T;

        [[nodiscard]] static Derived make() {
            return Derived::make_impl();
        }

        [[nodiscard]] static Derived make(const T& t) {
            return Derived::make_impl(t);
        }

        T& operator*() {
            return *static_cast<Derived>(*this);
        }

        T* operator->() {
            return &*static_cast<Derived>(*this);
        }

        [[nodiscard]] bool is_none() {
            return static_cast<Derived>(*this).is_none_impl();
        }

        friend Derived;
    };
}
