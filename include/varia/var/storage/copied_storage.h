#pragma once
#include <utility>

template<typename T = void>
class CopiedStorage {
public:
    using object_type = T;

    template<typename... Args>
    [[nodiscard]] static CopiedStorage make(Args... args) {
        return CopiedStorage{object_type(std::forward<Args>(args)...)};
    }

    [[nodiscard]] const object_type* get() const {
        return &mValue;
    }

    [[nodiscard]] object_type* get() {
        return &mValue;
    }

private:
    explicit CopiedStorage(const object_type& value) : mValue{value} {
    }

    object_type mValue{};
};
