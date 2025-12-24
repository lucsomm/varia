#pragma once

#include <type_traits>
#include "../../var/var.h"

namespace varia::objects {
    struct EmptyPolymorphic final {
        virtual ~EmptyPolymorphic() = default;
    };

    namespace concepts {
        template<typename T>
        concept Empty = std::is_empty_v<T> || (std::is_polymorphic_v<T> && sizeof(T) == sizeof(EmptyPolymorphic));

        template<typename T>
        concept InterfaceLike = Empty<T> && (!std::is_polymorphic_v<T> || std::is_abstract_v<T>);

        template<typename... Args>
        concept AllInterfaces = (true && ... && objects::concepts::InterfaceLike<Args>);

        template<typename T>
        concept VirtualDestructor = !std::is_polymorphic_v<T> || std::has_virtual_destructor_v<T>;
    }

    template<typename...>
    struct IsSingleInheritance {
        static constexpr bool value{true};
    };

    template<typename T1, typename T2, typename... Args>
    struct IsSingleInheritance<T1, T2, Args...> {
        static constexpr bool value{
            objects::concepts::InterfaceLike<T2> && (objects::concepts::InterfaceLike<Args> && ...)
        };
    };

    template<typename... Args>
    concept SingleInheritance = IsSingleInheritance<Args...>::value;
}
