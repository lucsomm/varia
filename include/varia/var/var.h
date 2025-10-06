#pragma once
#include "storage/storage.h"

namespace varia {
    template<objects::Object T, storage::Storage StorageT = storage::DefaultValueStorage<T> >
    class var {
    public:
        var(const T& value) : mValueStorage{StorageT::make(value)} {
        }

        var(const objects::Arithmetic auto value) : mValueStorage(StorageT::make(value)) {
        }

        var(const char* value) : mValueStorage{StorageT::make(value)} {
        }

        var& operator=(const T& value) {
            *mValueStorage = value;
            return *this;
        }

        operator T() {
            return *mValueStorage;
        }

    private:
        StorageT mValueStorage{};
    };

    template<objects::Arithmetic T>
    var(T) -> var<objects::Num>;

    var(const char*) -> var<objects::String>;
}

