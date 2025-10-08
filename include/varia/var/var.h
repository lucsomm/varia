#pragma once
#include <charconv>

#include "storage/storage.h"
#include "objects/none.h"

namespace varia {
    template<objects::Object T, storage::Storage StorageT = storage::DefaultStorage<T> >
    class var;

    template<typename>
    struct is_var : std::false_type {
    };

    template<objects::Object T>
    struct is_var<var<T> > : std::true_type {
    };

    template<typename T>
    concept Var = is_var<T>::value;

    using None = var<objects::None>;
    using Bool = var<objects::Bool>;
    using Int = var<objects::Int>;
    using Float = var<objects::Float>;
    using Num = var<objects::Num>;

    using String = var<objects::String>;

    template<typename T>
    concept Stringable = std::same_as<var<typename T::ValueType>, T> &&
                         (objects::Arithmetic<typename T::ValueType> ||
                          std::same_as<objects::String, typename T::ValueType>);

    template<objects::Object T, storage::Storage StorageT>
    class var {
    public:
        using ValueType = T;

        var() : mStorage{StorageT::make()} {
        }

        var(const T& object) : mStorage{StorageT::make(object)} {
        }

        var(const objects::Arithmetic auto object) : mStorage(StorageT::make(object)) {
        }

        var(const char* object) : mStorage{StorageT::make(object)} {
        }

        var& operator=(const T& object) {
            *this = std::move(var{object});
            return *this;
        }

        operator T() const {
            return get();
        }

        const T* operator->() const {
            return &get();
        }

        T* operator->() {
            return &get();
        }

        bool operator==([[maybe_unused]] const objects::None /*unused*/) const {
            return mStorage.is_none();
        }

        var& operator=([[maybe_unused]] const objects::None /*unused*/) {
            mStorage.reset();
            return *this;
        }

        operator objects::String() requires (std::integral<T>) {
            return std::to_string(get());
        }

        operator objects::String() requires (std::same_as<objects::Num, T>) {
            constexpr size_t buffer_size = std::numeric_limits<objects::Float>::max_digits10;
            std::string str(buffer_size, '\0');
            auto [ptr, ec] = std::to_chars(str.data(), str.data() + str.size(),
                                           get(), std::chars_format::general);
            str.resize(ptr - str.data());
            return str;
        }

        template<Var U>
        friend const U::ValueType& get(const U& v);

        friend String& operator+=(String& lhs, const objects::String& rhs);

    private:
        [[nodiscard]] const T& get() const {
            return *mStorage;
        }

        [[nodiscard]] T& get() {
            return *mStorage;
        }

        [[nodiscard]] const objects::String& get() const requires (std::same_as<objects::String, T>) {
            if (mStorage.is_none()) {
                return objects::None::none_string();
            }

            return *mStorage;
        }

        [[nodiscard]] objects::String& get() requires (std::same_as<objects::String, T>) {
            if (mStorage.is_none()) {
                return objects::None::none_string();
            }

            return *mStorage;
        }

        StorageT mStorage{};
    };

    template<objects::Arithmetic T>
    var(T) -> var<objects::Num>;

    var(const char*) -> var<objects::String>;

    template<Var T>
    const T::ValueType& get(const T& v) {
        return v.get();
    }

    std::ostream& operator<<(std::ostream& lhs, const Stringable auto& rhs) {
        lhs << get(rhs);
        return lhs;
    }

    inline String operator+(const String& lhs, const objects::String& rhs) {
        return String{get(lhs) + rhs};
    }

    inline String operator+(const objects::String& lhs, const String& rhs) {
        return String{lhs + get(rhs)};
    }

    inline String& operator+=(String& lhs, const objects::String& rhs) {
        lhs.get() += rhs;
        return lhs;
    }
}
