#pragma once
#include <cstddef>
#include <utility>

class IntIntPairHash {
public:
    inline std::size_t operator()(const std::pair<int,int>& p) const {
        return (static_cast<std::size_t>(p.first) << 32) + p.second;
    }
};