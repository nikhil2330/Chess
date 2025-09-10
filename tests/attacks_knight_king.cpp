#include <cassert>
#include "chess/types.hpp"
#include "chess/bitboard.hpp"
#include "chess/position.hpp"
#include "chess/attacks.hpp"

int main() {
    Position p; p.clear();

    // Knight from d4 → 8 moves; from a1 → 2 moves
    int d4 = get_index('d', 4);
    int a1 = get_index('a', 1);
    assert(bits_set_count(computeKnightMoves(WHITE, p, d4)) == 8);
    assert(bits_set_count(computeKnightMoves(WHITE, p, a1)) == 2);

    // King from d4 → 8 moves; from a1 → 3 moves
    assert(bits_set_count(computeKingMoves(WHITE, p, d4)) == 8);
    assert(bits_set_count(computeKingMoves(WHITE, p, a1)) == 3);

    return 0;
}
