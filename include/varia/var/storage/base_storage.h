#pragma once
#include "../objects/object_hierarchy.h"

namespace varia::storage {
    template<objects::Object T, typename Derived>
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

        const T& operator*() const {
            return *static_cast<const Derived&>(*this);
        }

        T& operator*() {
            return *static_cast<Derived&>(*this);
        }

        const T* operator->() const {
            return &*static_cast<const Derived&>(*this);
        }

        T* operator->() {
            return &*static_cast<Derived&>(*this);
        }

        [[nodiscard]] bool is_none() const {
            return static_cast<const Derived&>(*this).is_none_impl();
        }

        void reset() {
            return static_cast<Derived&>(*this).reset_impl();
        }

        friend Derived;
    };
}
