#pragma once
#include <optional>

#include "object_hierarchy.h"

namespace varia::objects {
    class Num : CopiedObject, StandardObject {
    public:
        explicit Num(const Arithmetic auto value) : mValue{static_cast<Float>(value)} {
        }

        explicit operator Int() const {
            if (!mValue.has_value()) {
                return Int{};
            }

            return static_cast<Int>(*mValue);
        }

        explicit operator Float() const {
            if (!mValue.has_value()) {
                return Float{};
            }

            return *mValue;
        }

        bool operator==(const Num& other) const {
            return mValue == other.mValue;
        }

    private:
        std::optional<Float> mValue;
    };
}
