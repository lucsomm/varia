#pragma once

#include <string>

#include "objects/object_hierarchy.h"
#include "storage/copied_storage.h"
#include "storage/immutable_shared_storage.h"
#include "storage/shared_storage.h"
#include "storage/storage.h"

namespace varia {
    template<typename T, template <typename > typename S = SharedStorage> requires Storage<S<std::decay_t<T> > >
    class var;

    template<typename>
    struct is_var : std::false_type {
    };

    template<typename T, template <typename > typename S>
    struct is_var<var<T, S> > : std::true_type {
    };

    template<typename T>
    concept Var = is_var<std::decay_t<T> >::value;

    template<typename T>
    struct Get {
        using type = T;
    };

    template<Var T>
    struct Get<T> {
        using type = T::object_type;
    };

    template<typename T>
    using get_t = Get<std::decay_t<T> >::type;

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
    concept Addable = [] {
        using std::operator+; // MSVC ADL compatability
        return requires(L lhs, R rhs)
        {
            { lhs + rhs } -> std::constructible_from<std::common_type_t<L, R> >;
        };
    }();

    template<typename L, typename R>
        requires Addable<get_t<L>, get_t<R> >
    auto operator+(const L& lhs, const R& rhs) {
        return std::common_type_t<L, R>{get(lhs) + get(rhs)};
    }

    template<Var L, typename R>
        requires Addable<get_t<L>, get_t<R> >
    L& operator+=(L& lhs, const R& rhs) {
        get(lhs) = get(lhs) + get(rhs);
        return lhs;
    }
}
