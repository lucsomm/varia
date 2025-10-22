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
        struct Get {
            using type = T;
        };

        template<Var T>
        struct Get<T> {
            using type = T::object_type;
        };
    }

    template<typename T>
    using get_t = detail::Get<std::decay_t<T> >::type;

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
    using Array = var<objects::Array<T> >;
    template<typename K, typename V>
    using Map = var<objects::Map<K, V> >;

    template<typename T>
    concept Arithmetic = std::is_arithmetic_v<std::decay_t<T> > || (
                             Var<T> && std::is_arithmetic_v<typename std::decay_t<T>::object_type>);

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

        var(const object_type& vt) : mStorage{storage_policy::make(vt)} {
        }

        template<typename... Args>
        var(Args... args) : mStorage{storage_policy::make(std::forward<Args>(args)...)} {
        }

        template<std::derived_from<object_type> Derived>
        var(const var<Derived>& from) : mStorage{from.get_storage()} {
        }

        var(const Arithmetic auto& from) requires std::same_as<object_type, objects::String> : mStorage{
            storage_policy::make(objects::to_string(get(from)))
        } {
        }

        [[nodiscard]] const storage_policy& get_storage() const noexcept {
            return mStorage;
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
    concept Addable = requires(L lhs, R rhs)
    {
        typename std::common_type_t<L, R>;
        { lhs + rhs } -> std::convertible_to<std::common_type_t<L, R> >;
    };

    template<typename L, typename R>
        requires Addable<get_t<L>, get_t<R> >
    auto operator+(const L& lhs, const R& rhs) {
        return std::common_type_t<L, R>(get(lhs) + get(rhs));
    }

    template<Var L, typename R>
        requires Addable<get_t<L>, get_t<R> >
    L& operator+=(L& lhs, const R& rhs) {
        get(lhs) = get(lhs) + get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    concept Subtractable = requires(L lhs, R rhs)
    {
        typename std::common_type_t<L, R>;
        { lhs - rhs } -> std::convertible_to<std::common_type_t<L, R> >;
    };

    template<typename L, typename R>
        requires Subtractable<get_t<L>, get_t<R> >
    auto operator-(const L& lhs, const R& rhs) {
        return std::common_type_t<L, R>(get(lhs) - get(rhs));
    }

    template<Var L, typename R>
        requires Subtractable<get_t<L>, get_t<R> >
    L& operator-=(L& lhs, const R& rhs) {
        get(lhs) = get(lhs) - get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    concept Multipliable = requires(L lhs, R rhs)
    {
        typename std::common_type_t<L, R>;
        { lhs * rhs } -> std::convertible_to<std::common_type_t<L, R> >;
    };

    template<typename L, typename R>
        requires Multipliable<get_t<L>, get_t<R> >
    auto operator*(const L& lhs, const R& rhs) {
        return std::common_type_t<L, R>(get(lhs) * get(rhs));
    }

    template<Var L, typename R>
        requires Multipliable<get_t<L>, get_t<R> >
    L& operator*=(L& lhs, const R& rhs) {
        get(lhs) = get(lhs) * get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    concept Dividable = requires(L lhs, R rhs)
    {
        typename std::common_type_t<L, R>;
        { lhs / rhs } -> std::convertible_to<std::common_type_t<L, R> >;
    };

    template<typename L, typename R>
        requires Dividable<get_t<L>, get_t<R> >
    auto operator/(const L& lhs, const R& rhs) {
        return std::common_type_t<L, R>(get(lhs) / get(rhs));
    }

    template<Var L, typename R>
        requires Dividable<get_t<L>, get_t<R> >
    L& operator/=(L& lhs, const R& rhs) {
        get(lhs) = get(lhs) / get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    concept Modable = requires(L lhs, R rhs)
    {
        typename std::common_type_t<L, R>;
        { lhs % rhs } -> std::convertible_to<std::common_type_t<L, R> >;
    };

    template<typename L, typename R>
        requires Modable<get_t<L>, get_t<R> >
    auto operator%(const L& lhs, const R& rhs) {
        return std::common_type_t<L, R>(get(lhs) % get(rhs));
    }

    template<Var L, typename R>
        requires Modable<get_t<L>, get_t<R> >
    L& operator%=(L& lhs, const R& rhs) {
        get(lhs) = get(lhs) % get(rhs);
        return lhs;
    }
}
