#pragma once

#include <string>

#include "objects/object_hierarchy.h"
#include "storage/copied_storage.h"
#include "storage/immutable_shared_storage.h"
#include "storage/shared_storage.h"
#include "storage/storage.h"

namespace varia {
    template<typename T, template <typename > typename S = SharedStorage> requires Storage<S<T> >
    class var;

    template<typename>
    struct is_var : std::false_type {
    };

    template<typename T, template <typename > typename S>
    struct is_var<var<T, S> > : std::true_type {
    };

    template<typename T>
    concept Var = is_var<T>::value;

    template<typename T>
    decltype(auto) get(T&& t) {
        return std::forward<T>(t);
    }

    template<Var T>
    const T::object_type& get(const T& t) {
        return *t;
    }

    template<Var T>
    T::object_type& get(T& t) {
        return *t;
    }

    using Bool = var<objects::Bool, CopiedStorage>;
    using Int = var<objects::Int, CopiedStorage>;
    using Float = var<objects::Float, CopiedStorage>;
    using Num = var<objects::Num, CopiedStorage>;
    using String = var<objects::String, ImmutableSharedStorage>;

    template<typename T>
    concept Arithmetic = std::is_arithmetic_v<T> || (Var<T> && std::is_arithmetic_v<typename T::object_type>);

    template<typename T, template <typename > typename S> requires Storage<S<T> >
    class var {
    public:
        using object_type = T;
        using storage_policy = S<T>;

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

        //
        // String coercion constructors
        //

        var(const Arithmetic auto& from) requires std::same_as<object_type, objects::String> : mStorage{
            storage_policy::make(std::to_string(get(from)))
        } {
        }

        [[nodiscard]] const storage_policy& get_storage() const {
            return mStorage;
        }

        const object_type* operator->() const {
            return mStorage.get();
        }

        object_type* operator->() {
            return mStorage.get();
        }

        const object_type& operator*() const {
            return *mStorage.get();
        }

        object_type& operator*() {
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

    template<typename L, typename R>
    concept LeftAddable = requires(L lhs, R rhs)
    {
        { lhs + rhs } -> std::convertible_to<L>;
        { lhs += rhs } -> std::same_as<L&>;
    };

    template<Var L, Var R>
        requires LeftAddable<typename L::object_type, typename R::object_type>
    L operator+(const L& lhs, const R& rhs) {
        return L{*lhs + *rhs};
    }

    template<Var L, typename R>
        requires LeftAddable<typename L::object_type, R>
    L operator+(const L& lhs, const R& rhs) {
        return L{*lhs + rhs};
    }

    template<typename L, Var R>
        requires LeftAddable<typename R::object_type, L>
    R operator+(const L& lhs, const R& rhs) {
        return R{lhs + *rhs};
    }

    template<Var L, Var R>
        requires LeftAddable<typename L::object_type, typename R::object_type>
    L& operator+=(L& lhs, const R& rhs) {
        *lhs = *lhs + *rhs;
        return lhs;
    }

    template<Var L, typename R>
        requires LeftAddable<typename L::object_type, R>
    L& operator+=(L& lhs, const R& rhs) {
        *lhs = *lhs + *rhs;
        return lhs;
    }
}
