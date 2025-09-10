#include <cassert>
#include <algorithm>
#include <vector>
#include <string>
#include "chess/position.hpp"
#include "chess/move.hpp"
#include "chess/movegen.hpp"
#include "chess/make_undo.hpp"
#include "chess/fen.hpp"
#include "chess/cli.hpp"

static Move pick_uci(Position& pos, const char* uci) {
    std::vector<Move> legal;
    generateLegalAllMoves(pos, legal);
    Move typed = convert_command(pos, std::string(uci));
    auto it = std::find(legal.begin(), legal.end(), typed);
    assert(it != legal.end());
    return *it;
}

int main() {
    Position p; p.start_position();
    std::string fen0 = saveFEN(p);

    Move m = pick_uci(p, "e2e4");
    makeMove(p, m);
    UndoMove(p);

    std::string fen1 = saveFEN(p);
    assert(fen1 == fen0);
    return 0;
}
