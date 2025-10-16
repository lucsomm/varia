#pragma once
#include <variant>

#include "object_hierarchy.h"

namespace varia::objects {
    class Num : CopiedObject, StandardObject {
    public:
        Num() = default;

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

        Num operator+(const Num& other) const {
            if (std::holds_alternative<None>(mValue) || std::holds_alternative<None>(other.mValue)) {
                return Num{};
            }

            if (std::holds_alternative<Float>(mValue) || std::holds_alternative<Float>(other.mValue)) {
                return Num{static_cast<Float>(*this) + static_cast<Float>(other)};
            }

            return Num{static_cast<Int>(*this) + static_cast<Int>(other)};
        }

    private:
        std::variant<None, Int, Float> mValue{none};
    };
}
