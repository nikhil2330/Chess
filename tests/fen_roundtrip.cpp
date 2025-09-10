#include <cassert>
#include <string>
#include "chess/position.hpp"
#include "chess/fen.hpp"

int main() {
    Position p;
    p.start_position();
    std::string fen = saveFEN(p);
    assert(fen == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    Position p2;
    bool ok = loadFEN(p2, fen);
    assert(ok);
    std::string fen2 = saveFEN(p2);
    assert(fen2 == fen);
    return 0;
}
