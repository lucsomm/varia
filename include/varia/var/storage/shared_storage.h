#pragma once
#include <memory>
#include <utility>

namespace varia {
    template<typename T>
    class SharedStorage {
    public:
        using object_type = T;
        using pointer = std::shared_ptr<object_type>;
        using const_pointer = pointer;

        template<typename... Args>
        [[nodiscard]] static SharedStorage make(Args&&... args) {
            return SharedStorage{std::make_shared<object_type>(std::forward<Args>(args)...)};
        }

        template<std::derived_from<object_type> Derived>
        explicit SharedStorage(const SharedStorage<Derived>& other) noexcept : mObject{other.get()} {
        }

        [[nodiscard]] const pointer& get() const noexcept {
            return mObject;
        }

        [[nodiscard]] pointer& get() noexcept {
            return mObject;
        }

    private:
        explicit SharedStorage(pointer ptr) noexcept : mObject{ptr} {
        }

        pointer mObject;
    };
}
