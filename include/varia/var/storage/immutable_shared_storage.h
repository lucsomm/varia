#pragma once
#include <memory>
#include <utility>

namespace varia {
    template<typename T>
    class ImmutableSharedStorage {
    public:
        using object_type = T;
        using pointer = std::shared_ptr<object_type>;
        using const_pointer = const pointer;

        template<typename... Args>
        [[nodiscard]] static ImmutableSharedStorage make(Args&&... args) {
            return ImmutableSharedStorage{std::make_shared<object_type>(std::forward<Args>(args)...)};
        }

        template<std::derived_from<object_type> Derived>
        explicit ImmutableSharedStorage(const ImmutableSharedStorage<Derived>& other) noexcept : mObject{
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
        explicit ImmutableSharedStorage(std::shared_ptr<object_type> ptr) noexcept : mObject{ptr} {
        }

        pointer mObject;
    };
}
