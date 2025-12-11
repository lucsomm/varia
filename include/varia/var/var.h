#pragma once

#include <string>
#include <algorithm>
#include <concepts>
#include <type_traits>
#include <utility>

#include "objects/object_hierarchy.h"
#include "objects/string_object.h"
#include "storage/default_storage.h"
#include "storage/shared_storage.h"
#include "storage/storage.h"
#include "storage/value_storage.h"

namespace varia {
    template<typename Obj, template <typename > typename S = DefaultStorage> requires concepts::Storage<S<std::decay_t<
        Obj> > >
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
            using type = typename T::object_type;
        };
    }

    template<typename T>
    using get_object_t = detail::GetObject<std::decay_t<T> >::type;

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
            using type = typename T::value_type;
        };

        template<typename T>
        using get_value_t = GetValue<get_object_t<T> >::type;
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

    namespace detail {
        template<typename T>
        struct GetStorage {
            using policy = ValueStorage<T>;
        };

        template<concepts::Var T>
        struct GetStorage<T> {
            using policy = typename T::storage_policy;
        };
    }

    template<typename T>
    using get_storage_policy_t = detail::GetStorage<std::decay_t<T> >::policy;

    namespace detail {
        template<typename T>
        struct IsValueStored {
            static constexpr bool value{std::same_as<get_storage_policy_t<T>, ValueStorage<get_object_t<T> > >};
        };

        template<typename T>
        struct IsShared {
            static constexpr bool value{std::same_as<get_storage_policy_t<T>, SharedStorage<get_object_t<T> > >};
        };
    }

    namespace concepts {
        template<typename T>
        concept ValueStored = detail::IsValueStored<std::decay_t<T> >::value;

        template<typename T>
        concept Shared = detail::IsShared<std::decay_t<T> >::value;
    }

    namespace detail {
        template<typename L, typename R>
        struct CommonVar {
            using object_type = std::common_type_t<get_object_t<L>, get_object_t<R> >;

            using type = std::conditional_t<!objects::concepts::Primitive<object_type> && (concepts::Shared<L> ||
                                                concepts::Shared<R>),
                var<object_type, SharedStorage>,
                var<object_type, ValueStorage>
            >;
        };
    }

    template<typename L, typename R>
    using common_var_t = detail::CommonVar<std::decay_t<L>, std::decay_t<R> >::type;

    namespace detail {
        template<typename T>
        struct DefaultStoragePolicy {
            using type =
            std::conditional_t<concepts::Var<T>,
                get_storage_policy_t<T>,
                std::conditional_t<objects::concepts::Primitive<T> || std::derived_from<T, Construct_Value>,
                    ValueStorage<T>,
                    SharedStorage<T>
                >
            >;
        };
    }

    template<typename T>
    using default_storage_policy_t = detail::DefaultStoragePolicy<T>::type;

    using Bool = var<objects::Bool>;
    using Int = var<objects::Int>;
    using Float = var<objects::Float>;
    using String = var<objects::String>;

    template<typename T>
    using Array = var<objects::Array<var<T> > >;
    template<typename K, typename V>
    using Map = var<objects::Map<var<K>, var<V> > >;

    using Value = var<Construct_Value>;
    using Shared = var<Construct_Shared>;

    namespace concepts {
        template<typename T>
        concept Int = std::integral<get_object_t<T> >;

        template<typename T>
        concept Float = std::floating_point<get_object_t<T> >;

        template<typename T>
        concept Arithmetic = objects::concepts::Arithmetic<get_object_t<T> >;

        template<typename T>
        concept StringLike = objects::concepts::StringLike<get_object_t<T> >;

        template<typename T>
        concept Formatable = objects::concepts::Formatable<get_object_t<T> >;

        template<typename T>
        concept FormatableVar = Var<T> && objects::concepts::Formatable<get_object_t<T> >;
    }

    template<typename T>
    constexpr decltype(auto) get(T&& t) noexcept {
        return std::forward<T>(t);
    }

    template<concepts::Var T>
    constexpr const T::object_type& get(const T& t) noexcept {
        return *t;
    }

    template<concepts::Var T>
    constexpr T::object_type& get(T& t) noexcept {
        return *t;
    }

    template<typename Obj, template <typename > typename S> requires concepts::Storage<S<std::decay_t<Obj> > >
    class var {
    public:
        using object_type = std::decay_t<Obj>;
        using storage_policy = std::conditional_t<std::same_as<S<object_type>, DefaultStorage<object_type> >,
            default_storage_policy_t<object_type>,
            S<object_type> >;

        static_assert(!(objects::concepts::Primitive<object_type> &&
                        std::same_as<storage_policy, SharedStorage<object_type> >),
                      "varia static assert: primitive objects should not be stored with SharedStorage");

    private:
        template<typename T>
        static constexpr bool is_float_to_int_v{concepts::Float<T> && std::integral<object_type>};

        template<typename T>
        static constexpr bool is_string_to_arithmetic_v{
            concepts::StringLike<T> && objects::concepts::Arithmetic<object_type>
        };

        template<typename T>
        static constexpr bool needs_explicit_conversion_v{is_float_to_int_v<T> || is_string_to_arithmetic_v<T>};

        template<typename T>
        static constexpr bool is_to_string_v{
            !objects::concepts::StringLike<T> && objects::concepts::String<object_type>
        };

        template<typename T>
        static constexpr bool needs_conversion_v{needs_explicit_conversion_v<T> || is_to_string_v<T>};

        template<typename T>
        static constexpr bool is_same_var_v{std::same_as<std::remove_cvref_t<T>, var>};

        template<typename T>
        static constexpr bool is_derived_var_v{concepts::Var<T> && std::derived_from<get_object_t<T>, object_type>};

    public:
        var() = default;

        ~var() = default;

        var(const var&) = default;

        var& operator=(const var&) = default;

        var(var&&) = default;

        var& operator=(var&&) = default;

        template<typename T>
        explicit (needs_explicit_conversion_v<T>)
        constexpr var(T&& t) requires (!is_same_var_v<T> && !is_derived_var_v<T>) : mStorage{
            storage_policy::make(convert_forward(std::forward<T>(t)))
        } {
        }

        template<concepts::Var T>
        var(const T& from) requires (!is_same_var_v<T> && is_derived_var_v<T>) : mStorage{
            get_storage(from)
        } {
        }

        template<typename T1, typename T2, typename... Args>
        constexpr var(T1&& t1, T1&& t2, Args&&... args) : mStorage{
            storage_policy::make(std::forward<T1>(t1), std::forward<T2>(t2), std::forward<Args>(args)...)
        } {
        }

        var(std::initializer_list<detail::get_value_t<object_type> > li)
            requires objects::concepts::Array<object_type> : mStorage(storage_policy::make(li)) {
        }

        // std::initializer_list<T> to objects::Array<var<T>>
        var(std::initializer_list<get_object_t<detail::get_value_t<object_type> > > li)
            requires (objects::concepts::Array<object_type> &&
                      concepts::Var<detail::get_value_t<object_type> >) : mStorage{storage_policy::make(li.size())} {
            std::transform(li.begin(), li.end(), object().begin(), [](const auto& elem) {
                return detail::get_value_t<object_type>{elem};
            });
        }

        constexpr operator const object_type&() const noexcept {
            return object();
        }

        constexpr operator object_type&() noexcept {
            return object();
        }

        constexpr storage_policy::const_pointer operator->() const noexcept {
            return mStorage.get();
        }

        constexpr storage_policy::pointer operator->() noexcept {
            return mStorage.get();
        }

        constexpr const object_type& operator*() const noexcept {
            return object();
        }

        constexpr object_type& operator*() noexcept {
            return object();
        }

        const auto& operator[](const Int index) const {
            return object().at(index);
        }

        auto& operator[](const Int index) {
            return object().at(index);
        }

        template<concepts::Var U>
        friend const get_storage_policy_t<U>& get_storage(const U& u);

    private:
        template<concepts::Float T>
        [[nodiscard]] constexpr object_type convert_forward(T&& t) requires is_float_to_int_v<T> {
            return static_cast<object_type>(get(std::forward<T>(t)));
        }

        template<concepts::StringLike T>
        [[nodiscard]] constexpr object_type convert_forward(T&& t) requires is_string_to_arithmetic_v<T> {
            return objects::to_arithmetic<object_type>(get(std::forward<T>(t)));
        }

        template<concepts::Formatable T>
        [[nodiscard]] constexpr object_type convert_forward(T&& t) requires is_to_string_v<T> {
            return objects::to_string(get(std::forward<T>(t)));
        }

        template<typename T>
        [[nodiscard]] constexpr decltype(auto) convert_forward(T&& t) requires (!needs_conversion_v<T>) {
            return std::forward<T>(t);
        }

        [[nodiscard]] constexpr const object_type& object() const {
            return *mStorage.get();
        }

        [[nodiscard]] constexpr object_type& object() {
            return *mStorage.get();
        }

        storage_policy mStorage{storage_policy::make()};
    };

    var(bool) -> var<objects::Bool, ValueStorage>;

    template<concepts::Int T>
    var(T) -> var<objects::Int, ValueStorage>;

    template<concepts::Float T>
    var(T) -> var<objects::Float, ValueStorage>;

    var(const char*) -> var<objects::String, ValueStorage>;

    var(std::string_view) -> var<objects::String, ValueStorage>;

    template<objects::concepts::Pointer T>
    var(T) -> var<T, ValueStorage>;

    template<objects::concepts::Arithmetic T>
    var(std::initializer_list<T>) -> var<objects::Array<var<T, ValueStorage> > >;

    template<objects::concepts::StringLike T>
    var(std::initializer_list<T>) -> var<objects::Array<var<T, ValueStorage> > >;

    template<objects::concepts::Pointer T> requires (!std::same_as<T, const char*>)
    var(std::initializer_list<T>) -> var<objects::Array<var<T, ValueStorage> > >;

    template<concepts::Var T>
    var(std::initializer_list<T>) -> var<objects::Array<T> >;

    template<typename T>
    var(std::initializer_list<T>) -> var<objects::Array<var<std::decay_t<T> > > >;

    template<objects::concepts::Array T>
    var(T) -> var<T>;

    template<concepts::Var U>
    [[nodiscard]] const get_storage_policy_t<U>& get_storage(const U& u) {
        return u.mStorage;
    }

    std::ostream& operator<<(std::ostream& os, const concepts::Var auto& v) {
        os << objects::to_string(get(v));
        return os;
    }

    template<template <typename > typename S>
    var<objects::String, S> operator+(const var<objects::String, S>& lhs, const concepts::Arithmetic auto& rhs) {
        return {get(lhs) + objects::to_string(get(rhs))};
    }

    template<template <typename > typename S>
    var<objects::String, S> operator+(const concepts::Arithmetic auto& lhs, const var<objects::String, S>& rhs) {
        return {objects::to_string(get(lhs)) + get(rhs)};
    }

    template<template <typename > typename S>
    var<objects::String, S>& operator+=(var<objects::String, S>& lhs, const concepts::Arithmetic auto& rhs) {
        get(lhs) += objects::to_string(get(rhs));
        return lhs;
    }

    String operator+(const concepts::FormatableVar auto& lhs, const char* rhs) {
        return String{objects::to_string(get(lhs)) + rhs};
    }

    String operator+(const char* lhs, const concepts::FormatableVar auto& rhs) {
        return String{lhs + objects::to_string(get(rhs))};
    }

    String operator+(const concepts::FormatableVar auto& lhs, const std::string_view rhs) {
        return String{objects::to_string(get(lhs)) + rhs};
    }

    String operator+(const std::string_view lhs, const concepts::FormatableVar auto& rhs) {
        return String{lhs + objects::to_string(get(rhs))};
    }

    String operator+(const concepts::FormatableVar auto& lhs, const std::string rhs) {
        return String{objects::to_string(get(lhs)) + rhs};
    }

    String operator+(const std::string lhs, const concepts::FormatableVar auto& rhs) {
        return String{lhs + objects::to_string(get(rhs))};
    }

    template<typename L, typename R>
    constexpr concepts::Var auto operator+(const L& lhs, const R& rhs) requires (
        (concepts::Var<L> || concepts::Var<R>) && requires { get(lhs) + get(rhs); }) {
        return common_var_t<L, R>(get(lhs) + get(rhs));
    }

    template<concepts::Var L, typename R>
    constexpr L& operator+=(L& lhs, const R& rhs) requires requires { get(lhs) += get(rhs); } {
        get(lhs) += get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    constexpr concepts::Var auto operator-(const L& lhs, const R& rhs) requires (
        (concepts::Var<L> || concepts::Var<R>) && requires { get(lhs) - get(rhs); }) {
        return common_var_t<L, R>(get(lhs) - get(rhs));
    }

    template<concepts::Var L, typename R>
    constexpr L& operator-=(L& lhs, const R& rhs) requires requires { get(lhs) -= get(rhs); } {
        get(lhs) -= get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    constexpr concepts::Var auto operator*(const L& lhs, const R& rhs) requires (
        (concepts::Var<L> || concepts::Var<R>) && requires { get(lhs) * get(rhs); }) {
        return common_var_t<L, R>(get(lhs) * get(rhs));
    }

    template<concepts::Var L, typename R>
    constexpr L& operator*=(L& lhs, const R& rhs) requires requires { get(lhs) *= get(rhs); } {
        get(lhs) *= get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    constexpr concepts::Var auto operator/(const L& lhs, const R& rhs) requires (
        (concepts::Var<L> || concepts::Var<R>) && requires { get(lhs) / get(rhs); }) {
        return common_var_t<L, R>(get(lhs) / get(rhs));
    }

    template<concepts::Var L, typename R>
    constexpr L& operator/=(L& lhs, const R& rhs) requires requires { get(lhs) /= get(rhs); } {
        get(lhs) /= get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    constexpr concepts::Var auto operator%(const L& lhs, const R& rhs) requires (
        (concepts::Var<L> || concepts::Var<R>) && requires { get(lhs) % get(rhs); }) {
        return common_var_t<L, R>(get(lhs) % get(rhs));
    }

    template<concepts::Var L, typename R>
    constexpr L& operator%=(L& lhs, const R& rhs) requires requires { get(lhs) %= get(rhs); } {
        get(lhs) %= get(rhs);
        return lhs;
    }

    template<concepts::Var T>
    constexpr T operator+(const T& v) requires requires { +get(v); } {
        return T{+get(v)};
    }

    template<concepts::Var T>
    constexpr T operator-(const T& v) requires requires { -get(v); } {
        return T{-get(v)};
    }
}

template<varia::concepts::Var T>
    requires varia::objects::concepts::Formatable<varia::get_object_t<T> >
struct VARIA_FORMAT_NS::formatter<T> : VARIA_FORMAT_NS::formatter<varia::get_object_t<T> > {
    template<typename FormatContext>
    auto format(const T& v, FormatContext& ctx) const {
        return VARIA_FORMAT_NS::formatter<varia::get_object_t<T> >::format(varia::get(v), ctx);
    }
};
