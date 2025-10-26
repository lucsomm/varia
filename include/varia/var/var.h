#pragma once

#include <string>
#include <algorithm>

#include "objects/object_hierarchy.h"
#include "objects/string_object.h"
#include "storage/copied_storage.h"
#include "storage/immutable_shared_storage.h"
#include "storage/shared_storage.h"
#include "storage/storage.h"

namespace varia {
    template<typename Obj, template <typename > typename S = SharedStorage> requires Storage<S<std::decay_t<Obj> > >
    class var;

    namespace detail {
        template<typename>
        struct is_var : std::false_type {
        };

        template<typename T, template <typename > typename S>
        struct is_var<var<T, S> > : std::true_type {
        };
    }

    namespace concepts {
        template<typename T>
        concept Var = detail::is_var<std::decay_t<T> >::value;
    }

    namespace detail {
        template<typename T>
        struct GetObject {
            using type = T;
        };

        template<concepts::Var T>
        struct GetObject<T> {
            using type = T::object_type;
        };
    }

    template<typename T>
    using get_object_type = detail::GetObject<std::decay_t<T> >::type;

    namespace detail {
        template<typename T>
        concept HasValueType = requires(T)
        {
            typename T::value_type;
        };

        template<typename T>
        struct GetValue {
            using type = T;
        };

        template<HasValueType T>
        struct GetValue<T> {
            using type = T::value_type;
        };

        template<typename T>
        using get_value_type = GetValue<T>::type;
    }

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

    using Bool = var<objects::Bool, CopiedStorage>;
    using Int = var<objects::Int, CopiedStorage>;
    using Float = var<objects::Float, CopiedStorage>;
    using Num = var<objects::Num, CopiedStorage>;
    using String = var<objects::String>;

    template<typename T>
    using Array = var<objects::Array<var<get_object_type<T> > > >;
    template<typename K, typename V>
    using Map = var<objects::Map<var<get_object_type<K> >, var<get_object_type<V> > > >;

    namespace concepts {
        template<typename T>
        concept Arithmetic = Var<T> && objects::concepts::Arithmetic<get_object_type<T> >;

        template<typename T>
        concept String = Var<T> && std::same_as<get_object_type<T>, objects::String>;

        template<typename T>
        concept Primitive = Arithmetic<T> || String<T>;

        template<typename T>
        concept StringCoercible = std::constructible_from<varia::String, get_object_type<T> >;
    }


    template<typename T>
    decltype(auto) get(T&& t) noexcept {
        return std::forward<T>(t);
    }

    template<concepts::Var T>
    const T::object_type& get(const T& t) noexcept {
        return *t;
    }

    template<concepts::Var T>
    T::object_type& get(T& t) noexcept {
        return *t;
    }

    template<typename Obj, template <typename > typename S> requires Storage<S<std::decay_t<Obj> > >
    class var {
    public:
        using object_type = std::decay_t<Obj>;
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

        var(std::initializer_list<detail::get_value_type<object_type> > li)
            requires objects::concepts::Array<object_type> : mStorage(storage_policy::make(li)) {
        }

        // std::initializer_list<T> to objects::Array<var<T>>
        var(std::initializer_list<get_object_type<detail::get_value_type<object_type> > > li)
            requires (objects::concepts::Array<object_type> && concepts::Var<detail::get_value_type<
                          object_type> >) : mStorage{
            storage_policy::make(li.size())
        } {
            std::transform(li.begin(), li.end(), object().begin(), [](const auto& elem) {
                return detail::get_value_type<object_type>{elem};
            });
        }

        var(const objects::concepts::Arithmetic auto& from) requires std::same_as<object_type, objects::String>
            : mStorage{storage_policy::make(objects::to_string(get(from)))} {
        }

        [[nodiscard]] const storage_policy& get_storage() const noexcept {
            return mStorage;
        }

        operator const object_type&()
        const
            noexcept {
            return *mStorage.get();
        }

        operator object_type&()
            noexcept {
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

        var operator+(const auto& other) requires requires { get(*this) + get(other); } {
            return var{get(*this) + get(other)};
        }

    private:
        const object_type& object() const {
            return *mStorage.get();
        }

        object_type& object() {
            return *mStorage.get();
        }

        storage_policy mStorage{storage_policy::make()};
    };

    var(bool) -> var<objects::Bool, CopiedStorage>;

    template<objects::concepts::Arithmetic T>
    var(T) -> var<objects::Num, CopiedStorage>;

    var(const char*) -> var<objects::String, ImmutableSharedStorage>;

    var(std::string_view) -> var<objects::String, ImmutableSharedStorage>;

    template<typename T>
    var(std::initializer_list<T>) -> var<objects::Array<var<get_object_type<T> > > >;

    std::ostream& operator<<(std::ostream& os, const concepts::Var auto& v) {
        os << objects::to_string(get(v));
        return os;
    }

    template<template <typename > typename S>
    var<objects::String, S> operator+(const var<objects::String, S>& lhs, const concepts::StringCoercible auto& rhs) {
        return {get(lhs) + objects::to_string(get(rhs))};
    }

    template<template <typename > typename S>
    var<objects::String, S> operator+(const concepts::StringCoercible auto& lhs, const var<objects::String, S>& rhs) {
        return {objects::to_string(get(lhs)) + get(rhs)};
    }

    template<template <typename > typename S>
    var<objects::String, S>& operator+=(var<objects::String, S>& lhs, const concepts::StringCoercible auto& rhs) {
        get(lhs) += objects::to_string(get(rhs));
        return lhs;
    }

    template<typename L, typename R>
    concepts::Var auto operator+(const L& lhs, const R& rhs) requires (
        (concepts::Var<L> || concepts::Var<R>) && requires { get(lhs) + get(rhs); }) {
        return var<std::common_type_t<get_object_type<L>, get_object_type<R> > >(get(lhs) + get(rhs));
    }

    template<concepts::Var L, typename R>
    L& operator+=(L& lhs, const R& rhs) requires requires { get(lhs) += get(rhs); } {
        get(lhs) += get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    concepts::Var auto operator-(const L& lhs, const R& rhs) requires (
        (concepts::Var<L> || concepts::Var<R>) && requires { get(lhs) - get(rhs); }) {
        return var<std::common_type_t<get_object_type<L>, get_object_type<R> > >(get(lhs) - get(rhs));
    }

    template<concepts::Var L, typename R>
    L& operator-=(L& lhs, const R& rhs) requires requires { get(lhs) -= get(rhs); } {
        get(lhs) -= get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    concepts::Var auto operator*(const L& lhs, const R& rhs) requires (
        (concepts::Var<L> || concepts::Var<R>) && requires { get(lhs) * get(rhs); }) {
        return var<std::common_type_t<get_object_type<L>, get_object_type<R> > >(get(lhs) * get(rhs));
    }

    template<concepts::Var L, typename R>
    L& operator*=(L& lhs, const R& rhs) requires requires { get(lhs) *= get(rhs); } {
        get(lhs) *= get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    concepts::Var auto operator/(const L& lhs, const R& rhs) requires (
        (concepts::Var<L> || concepts::Var<R>) && requires { get(lhs) / get(rhs); }) {
        return var<std::common_type_t<get_object_type<L>, get_object_type<R> > >(get(lhs) / get(rhs));
    }

    template<concepts::Var L, typename R>
    L& operator/=(L& lhs, const R& rhs) requires requires { get(lhs) /= get(rhs); } {
        get(lhs) /= get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    concepts::Var auto operator%(const L& lhs, const R& rhs) requires (
        (concepts::Var<L> || concepts::Var<R>) && requires { get(lhs) % get(rhs); }) {
        return var<std::common_type_t<get_object_type<L>, get_object_type<R> > >(get(lhs) % get(rhs));
    }

    template<concepts::Var L, typename R>
    L& operator%=(L& lhs, const R& rhs) requires requires { get(lhs) %= get(rhs); } {
        get(lhs) %= get(rhs);
        return lhs;
    }

    template<concepts::Var T>
    T operator+(const T& v) requires requires { +get(v); } {
        return T{+get(v)};
    }

    template<concepts::Var T>
    T operator-(const T& v) requires requires { -get(v); } {
        return T{-get(v)};
    }
}

template<varia::concepts::Var T>
    requires varia::objects::Formatable<varia::get_object_type<T> >
struct VARIA_FORMAT_NS::formatter<T> : VARIA_FORMAT_NS::formatter<std::string_view> {
    template<typename FormatContext>
    auto format(const T& v, FormatContext& ctx) const {
        return VARIA_FORMAT_NS::formatter<std::string_view>::format(varia::objects::to_string(varia::get(v)), ctx);
    }
};
