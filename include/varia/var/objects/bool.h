#pragma once
#include <cstdint>

#include "none.h"

namespace varia::objects {
    class Bool : CopiedObject, StandardObject {
    public:
        constexpr explicit Bool(const bool value) : mState{static_cast<State>(value)} {
        }

        constexpr explicit Bool([[maybe_unused]] const None /* unused */) : mState{} {
        }

        constexpr explicit operator bool() const {
            return static_cast<bool>(mState);
        }

    private:
        enum class State : uint8_t {
            False,
            True,
            None
        };

        State mState{State::None};
    };
}
