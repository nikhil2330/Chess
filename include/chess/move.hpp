#pragma once
#include <cstdint>
#include "chess/types.hpp"

struct Move
{
    int from;
    int to;
    uint16_t flags;
    uint8_t promo;
    int captured;
};

inline bool operator==(const Move &a, const Move &b)
{
    return a.from == b.from &&
           a.to == b.to &&
           a.flags == b.flags &&
           a.promo == b.promo;
}
