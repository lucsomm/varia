#pragma once

#include <string>

#include "objects/object_hierarchy.h"
#include "objects/string.h"
#include "storage/copied_storage.h"
#include "storage/immutable_shared_storage.h"
#include "storage/shared_storage.h"
#include "storage/storage.h"

namespace varia {
    template<typename T, template <typename > typename S = SharedStorage> requires Storage<S<std::decay_t<T> > >
    class var;

    namespace detail {
        template<typename>
        struct is_var : std::false_type {
        };

        template<typename T, template <typename > typename S>
        struct is_var<var<T, S> > : std::true_type {
        };
    }

    template<typename T>
    concept Var = detail::is_var<std::decay_t<T> >::value;

    namespace detail {
        template<typename T>
        struct GetObject {
            using type = T;
        };

        template<Var T>
        struct GetObject<T> {
            using type = T::object_type;
        };
    }

    template<typename T>
    using get_object_t = detail::GetObject<std::decay_t<T> >::type;

    template<typename T>
    decltype(auto) get(T&& t) noexcept {
        return std::forward<T>(t);
    }

    template<Var T>
    const T::object_type& get(const T& t) noexcept {
        return *t;
    }

    template<Var T>
    T::object_type& get(T& t) noexcept {
        return *t;
    }

    using Bool = var<objects::Bool, CopiedStorage>;
    using Int = var<objects::Int, CopiedStorage>;
    using Float = var<objects::Float, CopiedStorage>;
    using Num = var<objects::Num, CopiedStorage>;
    using String = var<objects::String, ImmutableSharedStorage>;
    template<typename T>
    using Array = var<objects::Array<var<get_object_t<T> > > >;
    template<typename K, typename V>
    using Map = var<objects::Map<var<get_object_t<K> >, var<get_object_t<V> > > >;

    template<typename T>
    concept Arithmetic = std::is_arithmetic_v<std::decay_t<T> > || (
                             Var<T> && std::is_arithmetic_v<typename std::decay_t<T>::object_type>);

    namespace detail {
        template<typename>
        struct is_array_object : std::false_type {
        };

        template<typename T, typename Alloc>
        struct is_array_object<objects::Array<T, Alloc> > : std::true_type {
        };

        template<typename T>
        inline constexpr bool is_array_object_v = is_array_object<T>::value;
    }

    template<typename T>
    concept ArrayObject = detail::is_array_object_v<std::remove_cvref_t<T> >;

    template<typename T, template <typename > typename S> requires Storage<S<std::decay_t<T> > >
    class var {
    public:
        using object_type = std::decay_t<T>;
        using storage_policy = S<object_type>;

        ~var() = default;

        var(const var&) = default;

        var& operator=(const var&) = default;

        var(var&&) = default;

        var& operator=(var&&) = default;

        template<typename... Args>
        var(Args... args) : mStorage{storage_policy::make(std::forward<Args>(args)...)} {
        }

        template<std::derived_from<object_type> Derived>
        var(const var<Derived>& from) : mStorage{from.get_storage()} {
        }

        template<typename U>
        var(std::initializer_list<U> li) requires ArrayObject<object_type> : mStorage{
            storage_policy::make(li)
        } {
        }

        var(const Arithmetic auto& from) requires std::same_as<object_type, objects::String> : mStorage{
            storage_policy::make(objects::to_string(get(from)))
        } {
        }

        [[nodiscard]] const storage_policy& get_storage() const noexcept {
            return mStorage;
        }

        operator const object_type&() const noexcept {
            return *mStorage.get();
        }

        operator object_type&() noexcept {
            return *mStorage.get();
        }

        const object_type* operator->() const noexcept {
            return mStorage.get();
        }

        object_type* operator->() noexcept {
            return mStorage.get();
        }

        const object_type& operator*() const noexcept {
            return *mStorage.get();
        }

        object_type& operator*() noexcept {
            return *mStorage.get();
        }

    private:
        storage_policy mStorage{};
    };

    var(bool) -> var<objects::Bool, CopiedStorage>;

    template<std::integral T>
    var(T) -> var<objects::Num, CopiedStorage>;

    template<std::floating_point T>
    var(T) -> var<objects::Num, CopiedStorage>;

    var(const char*) -> var<objects::String, ImmutableSharedStorage>;

    var(std::string_view) -> var<objects::String, ImmutableSharedStorage>;

    template<typename T>
    var(std::initializer_list<T>) -> var<objects::Array<T> >;

    std::ostream& operator<<(std::ostream& os, const Var auto& v) {
        os << objects::to_string(get(v));
        return os;
    }

    template<typename T>
    concept StringLike = std::same_as<T, String> || std::constructible_from<objects::String, T>;

    template<typename T>
    concept StringCoercible = !StringLike<T> && std::constructible_from<String, T>;

    String operator+(const String& lhs, const StringCoercible auto& rhs) {
        return String{get(lhs) + objects::to_string(get(rhs))};
    }

    String operator+(const StringCoercible auto& lhs, const String& rhs) {
        return String{objects::to_string(get(lhs)) + get(rhs)};
    }

    String& operator+=(String& lhs, const StringCoercible auto& rhs) {
        lhs = lhs + rhs;
        return lhs;
    }

    template<typename L, typename R>
    auto operator+(const L& lhs, const R& rhs) requires requires { get(lhs) + get(rhs); } {
        return var<std::common_type_t<get_object_t<L>, get_object_t<R> > >(get(lhs) + get(rhs));
    }

    template<Var L, typename R>
    L& operator+=(L& lhs, const R& rhs) requires requires { get(lhs) += get(rhs); } {
        get(lhs) += get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    auto operator-(const L& lhs, const R& rhs) requires requires { get(lhs) - get(rhs); } {
        return var<std::common_type_t<get_object_t<L>, get_object_t<R> > >(get(lhs) - get(rhs));
    }

    template<Var L, typename R>
    L& operator-=(L& lhs, const R& rhs) requires requires { get(lhs) -= get(rhs); } {
        get(lhs) -= get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    auto operator*(const L& lhs, const R& rhs) requires requires { get(lhs) * get(rhs); } {
        return var<std::common_type_t<get_object_t<L>, get_object_t<R> > >(get(lhs) * get(rhs));
    }

    template<Var L, typename R>
    L& operator*=(L& lhs, const R& rhs) requires requires { get(lhs) *= get(rhs); } {
        get(lhs) *= get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    auto operator/(const L& lhs, const R& rhs) requires requires { get(lhs) / get(rhs); } {
        return var<std::common_type_t<get_object_t<L>, get_object_t<R> > >(get(lhs) / get(rhs));
    }

    template<Var L, typename R>
    L& operator/=(L& lhs, const R& rhs) requires requires { get(lhs) /= get(rhs); } {
        get(lhs) /= get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    auto operator%(const L& lhs, const R& rhs) requires requires { get(lhs) % get(rhs); } {
        return var<std::common_type_t<get_object_t<L>, get_object_t<R> > >(get(lhs) % get(rhs));
    }

    template<Var L, typename R>
    L& operator%=(L& lhs, const R& rhs) requires requires { get(lhs) %= get(rhs); } {
        get(lhs) %= get(rhs);
        return lhs;
    }
}
