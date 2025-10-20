#pragma once
#include <memory>
#include <utility>

namespace varia {
    template<typename T>
    class SharedStorage {
    public:
        using object_type = T;

        template<typename... Args>
        [[nodiscard]] static SharedStorage make(Args... args) {
            return SharedStorage{std::make_shared<object_type>(std::forward<Args>(args)...)};
        }

        template<std::derived_from<object_type> Derived>
        explicit SharedStorage(const SharedStorage<Derived>& other) : mObject{other.get_shared_ptr()} {
        }

        [[nodiscard]] const std::shared_ptr<object_type>& get_shared_ptr() const {
            return mObject;
        }

        [[nodiscard]] const object_type* get() const {
            return mObject.get();
        }

        [[nodiscard]] object_type* get() {
            return mObject.get();
        }

    private:
        explicit SharedStorage(std::shared_ptr<object_type> ptr) : mObject{ptr} {
        }

        std::shared_ptr<object_type> mObject;
    };
}
