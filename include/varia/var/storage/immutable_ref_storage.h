#pragma once
#include <memory>
#include "base_storage.h"

namespace varia::storage {
    template<objects::Object T>
    class ImmutableRefStorage : public BaseStorage<T, ImmutableRefStorage<T> > {
    public:
        [[nodiscard]] static ImmutableRefStorage make_impl() {
            return ImmutableRefStorage{};
        }

        [[nodiscard]] static ImmutableRefStorage make_impl(const T& t) {
            return ImmutableRefStorage{std::make_shared<T>(t)};
        }

        [[nodiscard]] const T* get_impl() const {
            return mValue.get();
        }

        [[nodiscard]] T* get_impl() {
            if (mValue.use_count() > 1) {
                mValue = std::make_shared<T>(*mValue);
            }

            return mValue.get();
        }

        [[nodiscard]] bool is_none_impl() const {
            return mValue == nullptr;
        }

        void reset_impl() {
            mValue.reset();
        }

    private:
        explicit ImmutableRefStorage() = default;

        explicit ImmutableRefStorage(const std::shared_ptr<T> value) : mValue{std::move(value)} {
        }

        std::shared_ptr<T> mValue{};
    };
}


