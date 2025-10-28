#pragma once

#include <string>
#include <algorithm>
#include <concepts>
#include <type_traits>
#include <utility>

#include "objects/object_hierarchy.h"
#include "objects/string_object.h"
#include "storage/copied_storage.h"
#include "storage/immutable_shared_storage.h"
#include "storage/shared_storage.h"
#include "storage/storage.h"

namespace varia {
    template<typename Obj, template <typename > typename S = SharedStorage> requires concepts::Storage<S<std::decay_t<
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
            using type = typename T::value_type;
        };

        template<typename T>
        using get_value_type = GetValue<get_object_type<T> >::type;
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
            using policy = CopiedStorage<T>;
        };

        template<concepts::Var T>
        struct GetStorage<T> {
            using policy = typename T::storage_policy;
        };
    }

    template<typename T>
    using get_storage_policy = detail::GetStorage<std::decay_t<T> >::policy;

    namespace detail {
        template<typename T>
        struct IsCopied {
            static constexpr bool value{std::same_as<get_storage_policy<T>, CopiedStorage<get_object_type<T> > >};
        };

        template<typename T>
        struct IsImmutableShared {
            static constexpr bool value{
                std::same_as<get_storage_policy<T>, ImmutableSharedStorage<get_object_type<T> > >
            };
        };

        template<typename T>
        struct IsShared {
            static constexpr bool value{std::same_as<get_storage_policy<T>, SharedStorage<get_object_type<T> > >};
        };
    }

    namespace concepts {
        template<typename T>
        concept Copied = detail::IsCopied<std::decay_t<T> >::value;

        template<typename T>
        concept ImmutableShared = detail::IsImmutableShared<std::decay_t<T> >::value;

        template<typename T>
        concept Shared = detail::IsShared<std::decay_t<T> >::value;
    }

    namespace detail {
        template<typename L, typename R>
        struct CommonVar {
            using object_type = std::common_type_t<get_object_type<L>, get_object_type<R> >;

            using type = std::conditional_t<concepts::Shared<L> || concepts::Shared<R>,
                var<object_type>,
                std::conditional_t<concepts::ImmutableShared<L> || concepts::ImmutableShared<R>,
                    var<object_type, ImmutableSharedStorage>,
                    var<object_type, CopiedStorage>
                >
            >;
        };
    }

    template<typename L, typename R>
    using common_var_type = detail::CommonVar<std::decay_t<L>, std::decay_t<R> >::type;

    namespace detail {
        template<typename T>
        struct DefaultStoredVar {
            using type =
            std::conditional_t<concepts::Var<T>,
                T,
                std::conditional_t<objects::concepts::String<T>,
                    var<T, ImmutableSharedStorage>,
                    std::conditional_t<objects::concepts::Primitive<T>,
                        var<T, CopiedStorage>,
                        var<T>
                    >
                >
            >;
        };
    }

    template<typename T>
    using default_stored_var_type = detail::DefaultStoredVar<std::decay_t<T> >::type;

    using Bool = default_stored_var_type<objects::Bool>;
    using Int = default_stored_var_type<objects::Int>;
    using Float = default_stored_var_type<objects::Float>;
    using String = default_stored_var_type<objects::String>;

    template<typename T>
    using Array = var<objects::Array<default_stored_var_type<T> > >;
    template<typename K, typename V>
    using Map = var<objects::Map<default_stored_var_type<K>, default_stored_var_type<V> > >;

    namespace concepts {
        template<typename T>
        concept Int = std::integral<get_object_type<T> >;

        template<typename T>
        concept Float = std::floating_point<get_object_type<T> >;

        template<typename T>
        concept Arithmetic = objects::concepts::Arithmetic<get_object_type<T> >;

        template<typename T>
        concept StringLike = objects::concepts::StringLike<get_object_type<T> >;

        template<typename T>
        concept FormatableVar = Var<T> && objects::concepts::Formatable<get_object_type<T> >;
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
        using storage_policy = S<object_type>;

        static_assert(!(objects::concepts::Primitive<object_type> &&
                        std::same_as<storage_policy, SharedStorage<object_type> >),
                      "varia static assert: primitive objects should not be stored with SharedStorage");

        ~var() = default;

        var(const var&) = default;

        var& operator=(const var&) = default;

        var(var&&) = default;

        var& operator=(var&&) = default;

        template<typename... Args>
        constexpr var(Args... args) : mStorage{storage_policy::make(std::forward<Args>(args)...)} {
        }

        template<concepts::Var T>
        constexpr var(const T& from) requires std::derived_from<get_object_type<T>, object_type> : mStorage{
            from.get_storage()
        } {
        }

        var(std::initializer_list<detail::get_value_type<object_type> > li)
            requires objects::concepts::Array<object_type> : mStorage(storage_policy::make(li)) {
        }

        // std::initializer_list<T> to objects::Array<var<T>>
        var(std::initializer_list<get_object_type<detail::get_value_type<object_type> > > li)
            requires (objects::concepts::Array<object_type> &&
                      concepts::Var<detail::get_value_type<object_type> >) : mStorage{storage_policy::make(li.size())} {
            std::transform(li.begin(), li.end(), object().begin(), [](const auto& elem) {
                return detail::get_value_type<object_type>{elem};
            });
        }

        constexpr explicit var(const concepts::Float auto from) requires concepts::Int<object_type> : mStorage{
            storage_policy::make(static_cast<object_type>(get(from)))
        } {
        }

        explicit var(const concepts::StringLike auto& from) requires concepts::Int<object_type> : mStorage{
            storage_policy::make(objects::to_int(get(from)))
        } {
        }

        explicit var(const concepts::StringLike auto& from) requires concepts::Float<object_type> : mStorage{
            storage_policy::make(objects::to_float(get(from)))
        } {
        }

        var(const concepts::Arithmetic auto& from)
            requires std::same_as<object_type, objects::String>
            : mStorage{storage_policy::make(objects::to_string(get(from)))} {
        }

        [[nodiscard]] constexpr const storage_policy& get_storage() const noexcept {
            return mStorage;
        }

        constexpr operator const object_type&() const noexcept {
            return *mStorage.get();
        }

        constexpr operator object_type&() noexcept {
            return *mStorage.get();
        }

        constexpr const object_type* operator->() const noexcept {
            return mStorage.get();
        }

        constexpr object_type* operator->() noexcept {
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

    private:
        [[nodiscard]] constexpr const object_type& object() const {
            return *mStorage.get();
        }

        [[nodiscard]] constexpr object_type& object() {
            return *mStorage.get();
        }

        storage_policy mStorage{storage_policy::make()};
    };

    var(bool) -> var<objects::Bool, CopiedStorage>;

    template<concepts::Int T>
    var(T) -> var<objects::Int, CopiedStorage>;

    template<concepts::Float T>
    var(T) -> var<objects::Float, CopiedStorage>;

    var(const char*) -> var<objects::String, ImmutableSharedStorage>;

    var(std::string_view) -> var<objects::String, ImmutableSharedStorage>;

    template<objects::concepts::Pointer T>
    var(T) -> var<T, CopiedStorage>;

    template<objects::concepts::Arithmetic T>
    var(std::initializer_list<T>) -> var<objects::Array<var<T, CopiedStorage> > >;

    template<objects::concepts::StringLike T>
    var(std::initializer_list<T>) -> var<objects::Array<var<T, ImmutableSharedStorage> > >;

    template<objects::concepts::Pointer T> requires (!std::same_as<T, const char*>)
    var(std::initializer_list<T>) -> var<objects::Array<var<T, CopiedStorage> > >;

    template<concepts::Var T>
    var(std::initializer_list<T>) -> var<objects::Array<T> >;

    template<typename T>
    var(std::initializer_list<T>) -> var<objects::Array<var<std::decay_t<T> > > >;

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
        return common_var_type<L, R>(get(lhs) + get(rhs));
    }

    template<concepts::Var L, typename R>
    constexpr L& operator+=(L& lhs, const R& rhs) requires requires { get(lhs) += get(rhs); } {
        get(lhs) += get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    constexpr concepts::Var auto operator-(const L& lhs, const R& rhs) requires (
        (concepts::Var<L> || concepts::Var<R>) && requires { get(lhs) - get(rhs); }) {
        return common_var_type<L, R>(get(lhs) - get(rhs));
    }

    template<concepts::Var L, typename R>
    constexpr L& operator-=(L& lhs, const R& rhs) requires requires { get(lhs) -= get(rhs); } {
        get(lhs) -= get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    constexpr concepts::Var auto operator*(const L& lhs, const R& rhs) requires (
        (concepts::Var<L> || concepts::Var<R>) && requires { get(lhs) * get(rhs); }) {
        return common_var_type<L, R>(get(lhs) * get(rhs));
    }

    template<concepts::Var L, typename R>
    constexpr L& operator*=(L& lhs, const R& rhs) requires requires { get(lhs) *= get(rhs); } {
        get(lhs) *= get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    constexpr concepts::Var auto operator/(const L& lhs, const R& rhs) requires (
        (concepts::Var<L> || concepts::Var<R>) && requires { get(lhs) / get(rhs); }) {
        return common_var_type<L, R>(get(lhs) / get(rhs));
    }

    template<concepts::Var L, typename R>
    constexpr L& operator/=(L& lhs, const R& rhs) requires requires { get(lhs) /= get(rhs); } {
        get(lhs) /= get(rhs);
        return lhs;
    }

    template<typename L, typename R>
    constexpr concepts::Var auto operator%(const L& lhs, const R& rhs) requires (
        (concepts::Var<L> || concepts::Var<R>) && requires { get(lhs) % get(rhs); }) {
        return common_var_type<L, R>(get(lhs) % get(rhs));
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
    requires varia::objects::concepts::Formatable<varia::get_object_type<T> >
struct VARIA_FORMAT_NS::formatter<T> : VARIA_FORMAT_NS::formatter<varia::get_object_type<T> > {
    template<typename FormatContext>
    auto format(const T& v, FormatContext& ctx) const {
        return VARIA_FORMAT_NS::formatter<varia::get_object_type<T> >::format(varia::get(v), ctx);
    }
};
