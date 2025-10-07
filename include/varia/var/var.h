#pragma once
#include "storage/storage.h"
#include "objects/none.h"

namespace varia {
    template<objects::Object T, storage::Storage StorageT = storage::DefaultValueStorage<T> >
    class var;

    using None = var<objects::None>;
    using Bool = var<objects::Bool>;
    using Int = var<objects::Int>;
    using Float = var<objects::Float>;
    using Num = var<objects::Num>;

    using String = var<objects::String>;

    template<objects::Object T, storage::Storage StorageT>
    class var {
    public:
        var() : mValueStorage{StorageT::make()} {
        }

        var(const T& value) : mValueStorage{StorageT::make(value)} {
        }

        var(const objects::Arithmetic auto value) : mValueStorage(StorageT::make(value)) {
        }

        var(const char* value) : mValueStorage{StorageT::make(value)} {
        }

        var& operator=(const T& value) {
            *this = var{value};
            return *this;
        }

        operator T() {
            return *mValueStorage;
        }

        const T* operator->() const {
            return &*mValueStorage;
        }

        T* operator->() {
            return &*mValueStorage;
        }

        bool operator==([[maybe_unused]] const objects::None /*unused*/) const {
            return mValueStorage.is_none();
        }

        var& operator=([[maybe_unused]] const objects::None /*unused*/) {
            mValueStorage.reset();
            return *this;
        }

        operator objects::String() requires (std::same_as<objects::String, T>) {
            if (mValueStorage.is_none()) {
                return objects::None::none_string();
            }

            return *mValueStorage;
        }

        operator objects::String() requires (objects::Arithmetic<T>) {
            return std::to_string(*mValueStorage);
        }

    private:
        StorageT mValueStorage{};
    };

    template<objects::Arithmetic T>
    var(T) -> var<objects::Num>;

    var(const char*) -> var<objects::String>;
}

