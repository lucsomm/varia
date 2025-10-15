#pragma once

#include <iostream>

#include "storage/storage.h"
#include "objects/none.h"
#include "objects/string.h"

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
    concept Stringable =
            Var<T> && (objects::Arithmetic<typename T::ValueType> ||
                       std::same_as<typename T::ValueType, objects::String>);

    template<typename T>
    constexpr T&& get(T&& t) noexcept requires (!Var<T>) {
        return std::forward<T>(t);
    }

    template<Var T>
    const T::ValueType& get(const T& v) {
        return v.get();
    }

    template<objects::Object T, storage::Storage StorageT>
    class var {
    public:
        using ValueType = T;

        var(const var&) = default;

        var& operator=(const var&) = default;

        var(var&&) noexcept = default;

        var& operator=(var&&) noexcept = default;

        ~var() = default;

        var() : mStorage{StorageT::make()} {
        }

        var(const T& object) : mStorage{StorageT::make(object)} {
        }

        template<std::derived_from<T> Derived>
        var(const Derived& object) requires (objects::Referenced<T>) : mStorage{
            StorageT::make_polymorphic(object)
        } {
        }

        var([[maybe_unused]] const objects::None /*unused*/)
            requires (!std::same_as<objects::None, T>) : mStorage{StorageT::make()} {
        }

        var(const objects::ArithmeticNotBool auto object) requires (std::same_as<objects::Num, T>) : mStorage{
            StorageT::make(object)
        } {
        }

        var(const char* object) requires (std::same_as<objects::String, T>) : mStorage{StorageT::make(object)} {
        }

        //
        // Converting constructors
        //

        var(const objects::Stringable auto& from) requires (std::same_as<objects::String,
            T>) : mStorage{StorageT::make(objects::detail::to_string(from))} {
        }

        var(const Stringable auto& from) requires (std::same_as<objects::String, T>) : mStorage{
            StorageT::make(objects::detail::to_string(varia::get(from)))
        } {
        }

        explicit var(const String& from) requires (objects::Arithmetic<T>) : mStorage{
            StorageT::make(objects::detail::to_num(varia::get(from)))
        } {
        }

        operator T() const {
            return get();
        }

        operator std::string() const requires (!std::same_as<T, objects::String>) {
            if (mStorage.is_none()) {
                return objects::None::none_string();
            }

            return objects::detail::to_string(get());
        }

        const T* operator->() const {
            return &get();
        }

        T* operator->() {
            return &get();
        }

        Bool operator==([[maybe_unused]] const objects::None /*unused*/) const {
            return mStorage.is_none();
        }

        var& operator=([[maybe_unused]] const objects::None /*unused*/) {
            mStorage.reset();
            return *this;
        }

        template<Var U>
        friend Bool operator==(const U& lhs, const U& rhs);

        template<Var U>
        friend const U::ValueType& get(const U& v);

        friend String& operator+=(String& lhs, const String& rhs);

        friend String& operator+=(String& lhs, const char* rhs);

        friend String& operator+=(String& lhs, const objects::Stringable auto& rhs);

    private:
        [[nodiscard]] const T& get() const {
            if constexpr (std::same_as<T, objects::String>) {
                if (mStorage.is_none()) {
                    return objects::None::none_string();
                }

                return *mStorage;
            } else {
                return *mStorage;
            }
        }

        [[nodiscard]] T& get() {
            if constexpr (std::same_as<T, objects::String>) {
                if (mStorage.is_none()) {
                    return objects::None::none_string();
                }

                return *mStorage;
            } else {
                return *mStorage;
            }
        }

        StorageT mStorage{};
    };

    var(bool) -> var<objects::Bool>;

    template<objects::ArithmeticNotBool T>
    var(T) -> var<objects::Num>;

    var(const char*) -> var<objects::String>;

    var(std::string_view) -> var<objects::String>;

    Num id(const Var auto& v) {
        if (v == none) {
            return Num{none};
        }

        return Num{static_cast<objects::Int>(reinterpret_cast<uintptr_t>(&get(v)))};
    }

    template<Var U>
    Bool operator==(const U& lhs, const U& rhs) {
        if (rhs.mStorage.is_none() || lhs.mStorage.is_none()) {
            return rhs.mStorage.is_none() && lhs.mStorage.is_none();
        }

        return &lhs.get() == &rhs.get() || lhs.get() == rhs.get();
    }

    std::ostream& operator<<(std::ostream& lhs, const Stringable auto& rhs) {
        lhs << get(rhs);
        return lhs;
    }

    inline String operator+(const String& lhs, const String& rhs) {
        return String{get(lhs) + get(rhs)};
    }

    String operator+(const String& lhs, const objects::Stringable auto& rhs) {
        return lhs + String{rhs};
    }

    String operator+(const Stringable auto& lhs, const char* rhs) {
        return String{get(String{lhs}) + rhs};
    }

    String operator+(const char* lhs, const Stringable auto& rhs) {
        return String{lhs + get(String{rhs})};
    }

    inline String& operator+=(String& lhs, const String& rhs) {
        lhs.get() += get(rhs);
        return lhs;
    }

    inline String& operator+=(String& lhs, const char* rhs) {
        lhs.get() += rhs;
        return lhs;
    }

    String& operator+=(String& lhs, const objects::Stringable auto& rhs) {
        lhs.get() += objects::detail::to_string(rhs);
        return lhs;
    }
}
