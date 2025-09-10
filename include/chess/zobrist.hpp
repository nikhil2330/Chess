#pragma once
#include <cstdint>
#include "chess/types.hpp"   

struct Position;             

namespace Zobrist {

extern std::uint64_t PIECE_SQ[12][64];
extern std::uint64_t CASTLING[4];
extern std::uint64_t EP_FILE[8];
extern std::uint64_t SIDE;

void init();

std::uint64_t compute(const Position& pos);

inline int piece_index(Piece p) {
    switch (p) {
        case WP: return 0;  case WN: return 1;  case WB: return 2;
        case WR: return 3;  case WQ: return 4;  case WK: return 5;
        case BP: return 6;  case BN: return 7;  case BB: return 8;
        case BR: return 9;  case BQ: return 10; case BK: return 11;
        default: return -1;
    }
}

} // namespace Zobrist
