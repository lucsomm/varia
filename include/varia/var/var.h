#pragma once

#include <string>
#include "storage/copied_storage.h"
#include "storage/shared_storage.h"
#include "storage/storage.h"

namespace varia {
    template<typename T, template <typename > typename S = SharedStorage> requires Storage<S<T> >
    class var;

    using Bool = var<bool, CopiedStorage>;
    using Num = var<double, CopiedStorage>;
    using String = var<std::string>;

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

        var(const object_type& vt) : mValue{storage_policy::make(vt)} {
        }

        template<typename... Args>
        var(Args... args) : mValue{storage_policy::make(std::forward<Args>(args)...)} {
        }

        template<std::derived_from<object_type> Derived>
        var(const var<Derived>& from) requires std::is_polymorphic_v<object_type> : mValue{from.get_storage()} {
        }

        [[nodiscard]] const storage_policy& get_storage() const {
            return mValue;
        }

        const object_type* operator->() const {
            return mValue.get();
        }

        object_type* operator->() {
            return mValue.get();
        }

    private:
        storage_policy mValue{};
    };

    var(bool) -> var<bool, CopiedStorage>;

    template<std::integral T>
    var(T) -> var<double, CopiedStorage>;

    template<std::floating_point T>
    var(T) -> var<double, CopiedStorage>;

    var(const char*) -> var<std::string>;

    var(std::string_view) -> var<std::string>;
}
