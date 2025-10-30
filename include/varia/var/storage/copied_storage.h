#pragma once
#include <utility>

template<typename T = void>
class CopiedStorage {
public:
    using object_type = T;

    template<typename... Args>
    [[nodiscard]] constexpr static CopiedStorage make(Args&&... args) {
        return CopiedStorage{object_type{std::forward<Args>(args)...}};
    }

    [[nodiscard]] constexpr const object_type* get() const noexcept {
        return &mObject;
    }

    [[nodiscard]] constexpr object_type* get() noexcept {
        return &mObject;
    }

private:
    explicit constexpr CopiedStorage(const object_type& value) noexcept : mObject{value} {
    }

    object_type mObject{};
};
