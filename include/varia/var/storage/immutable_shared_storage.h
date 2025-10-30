#pragma once
#include <memory>
#include <utility>

namespace varia {
    template<typename T>
    class ImmutableSharedStorage {
    public:
        using object_type = T;

        template<typename... Args>
        [[nodiscard]] static ImmutableSharedStorage make(Args&&... args) {
            return ImmutableSharedStorage{std::make_shared<object_type>(std::forward<Args>(args)...)};
        }

        template<std::derived_from<object_type> Derived>
        explicit ImmutableSharedStorage(const ImmutableSharedStorage<Derived>& other) noexcept : mObject{
            other.get_shared_ptr()
        } {
        }

        [[nodiscard]] const std::shared_ptr<object_type>& get_shared_ptr() const noexcept {
            return mObject;
        }

        [[nodiscard]] const object_type* get() const noexcept {
            return mObject.get();
        }

        [[nodiscard]] object_type* get() {
            if (mObject.use_count() > 1) {
                mObject = std::make_shared<object_type>(*mObject);
            }

            return mObject.get();
        }

    private:
        explicit ImmutableSharedStorage(std::shared_ptr<object_type> ptr) noexcept : mObject{ptr} {
        }

        std::shared_ptr<object_type> mObject;
    };
}
