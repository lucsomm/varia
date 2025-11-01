#pragma once
#include <utility>

template<typename T = void>
class CopiedStorage {
public:
    using object_type = T;
    using pointer = object_type*;
    using const_pointer = const object_type*;

    template<typename... Args>
    [[nodiscard]] constexpr static CopiedStorage make(Args&&... args) {
        return CopiedStorage{object_type{std::forward<Args>(args)...}};
    }

    [[nodiscard]] constexpr const_pointer get() const noexcept {
        return &mObject;
    }

    [[nodiscard]] constexpr pointer get() noexcept {
        return &mObject;
    }

private:
    explicit constexpr CopiedStorage(const object_type& value) noexcept : mObject{value} {
    }

    object_type mObject{};
};
