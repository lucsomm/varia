#pragma once
#include "base_storage.h"

namespace varia::storage {
    template<objects::Object T>
    class CopiedStorage : public BaseStorage<T, CopiedStorage<T> > {
    public:
        [[nodiscard]] static CopiedStorage make_impl() {
            return CopiedStorage{};
        }

        [[nodiscard]] static CopiedStorage make_impl(const T& t) {
            return CopiedStorage{t};
        }

        T& operator*() {
            return mValue;
        }

        T* operator->() {
            return &mValue;
        }

        [[nodiscard]] bool is_none_impl() const {
            return mValue == T{};
        }

    private:
        explicit CopiedStorage() = default;

        explicit CopiedStorage(const T& value) : mValue{std::move(value)} {
        }

        T mValue{};
    };
}
