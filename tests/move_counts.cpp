#include <cassert>
#include <vector>
#include <algorithm>
#include <string>
#include "chess/position.hpp"
#include "chess/move.hpp"
#include "chess/movegen.hpp"
#include "chess/make_undo.hpp"
#include "chess/cli.hpp"

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

    std::vector<Move> legal;
    generateLegalAllMoves(p, legal);
    assert(legal.size() == 20); // start position

    play(p, "e2e4"); // white plays
    legal.clear();
    generateLegalAllMoves(p, legal);
    assert(legal.size() == 20); // black has 20 replies

    return 0;
}
