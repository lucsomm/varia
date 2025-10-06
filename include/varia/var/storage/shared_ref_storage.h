#pragma once
#include <memory>
#include "base_storage.h"

namespace varia::storage {
    template<objects::VarCompatible T>
    class SharedRefStorage : public BaseStorage<T, SharedRefStorage<T> > {
    public:
        [[nodiscard]] static SharedRefStorage make_impl() {
            return SharedRefStorage{};
        }

        [[nodiscard]] static SharedRefStorage make_impl(const T& t) {
            return SharedRefStorage{std::make_shared<T>(t)};
        }

        T& operator*() {
            return *mValue;
        }

        T* operator->() {
            return &*mValue;
        }

        [[nodiscard]] bool is_none_impl() const {
            return mValue == nullptr;
        }

    private:
        explicit SharedRefStorage() = default;

        explicit SharedRefStorage(const std::shared_ptr<T> value) : mValue{std::move(value)} {
        }

        std::shared_ptr<T> mValue{};
    };
}
