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

    template<typename T>
    concept ArrayObject = detail::is_array_object_v<std::remove_cvref_t<T> >;

    template<typename T1, typename T2>
    concept SameObject = std::same_as<var<get_object_type<T1> >, var<get_object_type<T2> > >;

    template<typename T>
    concept Arithmetic = std::is_arithmetic_v<std::decay_t<T> > || (
                             Var<T> && std::is_arithmetic_v<typename std::decay_t<T>::object_type>);

    using Bool = var<objects::Bool, CopiedStorage>;
    using Int = var<objects::Int, CopiedStorage>;
    using Float = var<objects::Float, CopiedStorage>;
    using Num = var<objects::Num, CopiedStorage>;
    using String = var<objects::String, ImmutableSharedStorage>;
    template<typename T>
    using Array = var<objects::Array<var<get_object_type<T> > > >;
    template<typename K, typename V>
    using Map = var<objects::Map<var<get_object_type<K> >, var<get_object_type<V> > > >;

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
            requires ArrayObject<object_type> : mStorage(storage_policy::make(li)) {
        }

        // std::initializer_list<T> to objects::Array<var<T>>
        var(std::initializer_list<get_object_type<detail::get_value_type<object_type> > > li)
            requires (ArrayObject<object_type> && Var<detail::get_value_type<object_type> >) : mStorage{
            storage_policy::make(li.size())
        } {
            std::transform(li.begin(), li.end(), object().begin(), [](const auto& elem) {
                return detail::get_value_type<object_type>{elem};
            });
        }

        var(const Arithmetic auto& from) requires std::same_as<object_type, objects::String>
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

    template<std::integral T>
    var(T) -> var<objects::Num, CopiedStorage>;

    template<std::floating_point T>
    var(T) -> var<objects::Num, CopiedStorage>;

    var(const char*) -> var<objects::String, ImmutableSharedStorage>;

    var(std::string_view) -> var<objects::String, ImmutableSharedStorage>;

    template<typename T>
    var(std::initializer_list<T>) -> var<objects::Array<var<get_object_type<T> > > >;

    std::ostream& operator<<(std::ostream& os, const Var auto& v) {
        os << objects::to_string(get(v));
        return os;
    }

    template<typename T>
    concept StringLike = std::same_as<T, String> || std::constructible_from<objects::String, T>;

    template<typename T>
    concept StringCoercible = !StringLike<T> && std::constructible_from<String, T>;

    template<typename T>
    String operator+(const T& lhs, const StringCoercible auto& rhs)
        requires (Var<T> && std::same_as<get_object_type<T>, objects::String>) {
        return String{get(lhs) + objects::to_string(get(rhs))};
    }

    template<typename T>
    String operator+(const StringCoercible auto& lhs, const T& rhs)
        requires (Var<T> && std::same_as<get_object_type<T>, objects::String>) {
        return String{objects::to_string(get(lhs)) + get(rhs)};
    }

    String& operator+=(String& lhs, const StringCoercible auto& rhs) {
        lhs = lhs + rhs;
        return lhs;
    }

    template<typename L, typename R>
    Var auto operator+(const L& lhs, const R& rhs) requires ((Var<L> || Var<R>) && requires { get(lhs) + get(rhs); }) {
        return var<std::common_type_t<get_object_type<L>, get_object_type<R> > >(get(lhs) + get(rhs));
    }

    template<Var L, typename R>
    L& operator+=(L& lhs, const R& rhs) requires requires { get(lhs) += get(rhs); } {
        get(lhs) += get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    Var auto operator-(const L& lhs, const R& rhs) requires ((Var<L> || Var<R>) && requires { get(lhs) - get(rhs); }) {
        return var<std::common_type_t<get_object_type<L>, get_object_type<R> > >(get(lhs) - get(rhs));
    }

    template<Var L, typename R>
    L& operator-=(L& lhs, const R& rhs) requires requires { get(lhs) -= get(rhs); } {
        get(lhs) -= get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    Var auto operator*(const L& lhs, const R& rhs) requires ((Var<L> || Var<R>) && requires { get(lhs) * get(rhs); }) {
        return var<std::common_type_t<get_object_type<L>, get_object_type<R> > >(get(lhs) * get(rhs));
    }

    template<Var L, typename R>
    L& operator*=(L& lhs, const R& rhs) requires requires { get(lhs) *= get(rhs); } {
        get(lhs) *= get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    Var auto operator/(const L& lhs, const R& rhs) requires ((Var<L> || Var<R>) && requires { get(lhs) / get(rhs); }) {
        return var<std::common_type_t<get_object_type<L>, get_object_type<R> > >(get(lhs) / get(rhs));
    }

    template<Var L, typename R>
    L& operator/=(L& lhs, const R& rhs) requires requires { get(lhs) /= get(rhs); } {
        get(lhs) /= get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    Var auto operator%(const L& lhs, const R& rhs) requires ((Var<L> || Var<R>) && requires { get(lhs) % get(rhs); }) {
        return var<std::common_type_t<get_object_type<L>, get_object_type<R> > >(get(lhs) % get(rhs));
    }

    template<Var L, typename R>
    L& operator%=(L& lhs, const R& rhs) requires requires { get(lhs) %= get(rhs); } {
        get(lhs) %= get(rhs);
        return lhs;
    }

    template<Var T>
    T operator+(const T& v) requires requires { +get(v); } {
        return T{+get(v)};
    }

    template<Var T>
    T operator-(const T& v) requires requires { -get(v); } {
        return T{-get(v)};
    }
}

template<varia::Var T>
    requires varia::objects::Formatable<varia::get_object_type<T> >
struct VARIA_FORMAT_NS::formatter<T> : VARIA_FORMAT_NS::formatter<std::string_view> {
    template<typename FormatContext>
    auto format(const T& v, FormatContext& ctx) const {
        return VARIA_FORMAT_NS::formatter<std::string_view>::format(varia::objects::to_string(varia::get(v)), ctx);
    }
};
