#include <cassert>
#include <algorithm>
#include <vector>
#include <string>
#include "chess/position.hpp"
#include "chess/move.hpp"
#include "chess/movegen.hpp"
#include "chess/make_undo.hpp"
#include "chess/cli.hpp"
#include "chess/bitboard.hpp"

static void play(Position& pos, const char* uci) {
    std::vector<Move> legal;
    generateLegalAllMoves(pos, legal);
    Move typed = convert_command(pos, std::string(uci));
    auto it = std::find(legal.begin(), legal.end(), typed);
    assert(it != legal.end());
    makeMove(pos, *it);
}

int main() {
    Position p; p.start_position();
    int black_pawns_before = bits_set_count(p.p);

    // Sequence to enable e.p.: 1.e2e4 a7a6 2.e4e5 d7d5 3.e5d6 e.p.
    play(p, "e2e4");
    play(p, "a7a6");
    play(p, "e4e5");
    play(p, "d7d5");

    // Now en passant should be legal
    std::vector<Move> legal;
    generateLegalAllMoves(p, legal);
    Move ep = convert_command(p, "e5d6");
    auto it = std::find(legal.begin(), legal.end(), ep);
    assert(it != legal.end()); // en passant exists
    makeMove(p, *it);

    // Black pawn count decreased by 1
    int black_pawns_after = bits_set_count(p.p);
    assert(black_pawns_after == black_pawns_before - 1);

    // EP square cleared
    assert(p.en_passant == -1);
    return 0;
}
