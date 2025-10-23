#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace varia::objects {
    struct CopiedObject {
    };

    struct ImmutableSharedObject {
    };

    struct SharedObject {
    };

    using Bool = bool;
    using Int = int64_t;
    using Float = double;
    using Num = double;
    using String = std::string;
    template<typename T, typename Alloc = std::allocator<T> >
    using Array = std::vector<T, Alloc>;
    template<typename K, typename V>
    using Map = std::unordered_map<K, V>;
}

