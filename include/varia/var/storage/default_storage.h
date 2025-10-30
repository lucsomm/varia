#pragma once

namespace varia {
    template<typename T>
    struct DefaultStorage {
        using object_type = T;

        DefaultStorage() = delete;

        [[maybe_unused]] constexpr const object_type* get() const noexcept {
            return {};
        }

        [[maybe_unused]] constexpr object_type* get() noexcept {
            return {};
        }
    };
}
