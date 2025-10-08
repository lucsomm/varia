#pragma once
#include <memory>
#include "base_storage.h"

namespace varia::storage {
    template<objects::Object T>
    class SharedRefStorage : public BaseStorage<T, SharedRefStorage<T> > {
    public:
        [[nodiscard]] static SharedRefStorage make_impl() {
            return SharedRefStorage{};
        }

        [[nodiscard]] static SharedRefStorage make_impl(const T& t) {
            return SharedRefStorage{std::make_shared<T>(t)};
        }

        [[nodiscard]] const T* get_impl() const {
            return mValue.get();
        }

        [[nodiscard]] T* get_impl() {
            return mValue.get();
        }

        [[nodiscard]] bool is_none_impl() const {
            return mValue == nullptr;
        }

        void reset_impl() {
            mValue.reset();
        }

    private:
        explicit SharedRefStorage() = default;

        explicit SharedRefStorage(const std::shared_ptr<T> value) : mValue{std::move(value)} {
        }

        std::shared_ptr<T> mValue{};
    };
}
