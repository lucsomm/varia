#pragma once

namespace varia {
    template<typename T>
    struct DefaultStorage {
        using object_type = T;
        using pointer = object_type*;
        using const_pointer = const object_type*;

        DefaultStorage() = delete;

        [[maybe_unused]] constexpr const_pointer get() const noexcept {
            return {};
        }

        [[maybe_unused]] constexpr object_type* get() noexcept {
            return {};
        }
    };
}
