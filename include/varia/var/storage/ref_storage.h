#pragma once
#include <memory>
#include <utility>

namespace varia {
    template<typename T>
    class RefStorage {
    public:
        using object_type = T;
        using pointer = std::shared_ptr<object_type>;
        using const_pointer = const pointer;

        template<typename... Args>
        [[nodiscard]] static RefStorage make(Args&&... args) {
            return RefStorage{std::make_shared<object_type>(std::forward<Args>(args)...)};
        }

        template<std::derived_from<object_type> Derived>
        explicit RefStorage(const RefStorage<Derived>& other) noexcept : mObject{other.get()} {
        }

        [[nodiscard]] const_pointer& get() const noexcept {
            return mObject;
        }

        [[nodiscard]] pointer& get() noexcept {
            return mObject;
        }

    private:
        explicit RefStorage(pointer ptr) noexcept : mObject{ptr} {
        }

        pointer mObject;
    };
}
