#pragma once
#include <variant>

#include "object_hierarchy.h"

namespace varia::objects {
    class Num : CopiedObject, StandardObject {
    public:
        explicit Num(const Arithmetic auto value) : mValue(value) {
        }

        explicit operator Int() const {
            if (std::holds_alternative<Int>(mValue)) {
                return std::get<Int>(mValue);
            }

            if (std::holds_alternative<Float>(mValue)) {
                return static_cast<Int>(std::get<Float>(mValue));
            }

            return Int{};
        }

        explicit operator Float() const {
            if (std::holds_alternative<Float>(mValue)) {
                return std::get<Float>(mValue);
            }

            if (std::holds_alternative<Int>(mValue)) {
                return static_cast<Float>(std::get<Int>(mValue));
            }

            return Float{};
        }

        bool operator==(const Num& other) const {
            return static_cast<Float>(*this) == static_cast<Float>(other);
        }

    private:
        std::variant<None, Int, Float> mValue{none};
    };
}
