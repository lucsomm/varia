#pragma once

#include <memory>
#include <type_traits>

namespace varia {
    constexpr size_t trivial_size_max{32};

    template<typename T>
    concept NonTrivial = !std::is_trivially_copyable_v<T> || sizeof(T) > trivial_size_max;

    template<typename T /* , bool ForceInPlace = false */>
    class ValStorage {
    public:
        using object_type = T;
        using pointer = object_type*;
        using const_pointer = const object_type*;

        template<typename... Args>
        [[nodiscard]] static constexpr ValStorage make(Args&&... args) {
            return ValStorage{std::forward<Args>(args)...};
        }

        [[nodiscard]] constexpr const_pointer get() const noexcept {
            return &mObject;
        }

        [[nodiscard]] constexpr pointer get() noexcept {
            return &mObject;
        }

    private:
        template<typename... Args>
        explicit constexpr ValStorage(Args... args) noexcept : mObject(std::forward<Args>(args)...) {
        }

        object_type mObject{};
    };

    template<NonTrivial T>
    class ValStorage<T /* , false */> {
    public:
        using object_type = T;
        using pointer = std::shared_ptr<object_type>;
        using const_pointer = const pointer;

        template<typename... Args>
        [[nodiscard]] static ValStorage make(Args&&... args) {
            return ValStorage{std::make_shared<object_type>(std::forward<Args>(args)...)};
        }

        template<std::derived_from<object_type> Derived>
        explicit ValStorage(const ValStorage<Derived>& other) noexcept : mObject{
            other.get()
        } {
        }

        [[nodiscard]] const_pointer& get() const noexcept {
            return mObject;
        }

        [[nodiscard]] pointer& get() {
            if (mObject.use_count() > 1) {
                mObject = std::make_shared<object_type>(*mObject);
            }

            return mObject;
        }

    private:
        explicit ValStorage(std::shared_ptr<object_type> ptr) noexcept : mObject{ptr} {
        }

        pointer mObject;
    };

    template<typename T>
    using ValClass = ValStorage<T>;
}
