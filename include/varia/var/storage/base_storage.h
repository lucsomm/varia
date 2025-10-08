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

        [[nodiscard]] const T* get() const {
            return static_cast<const Derived*>(this)->get_impl();
        }

        [[nodiscard]] T* get() {
            return static_cast<Derived*>(this)->get_impl();
        }

        const T& operator*() const {
            return *get();
        }

        T& operator*() {
            return *get();
        }

        const T* operator->() const {
            return get();
        }

        T* operator->() {
            return get();
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
