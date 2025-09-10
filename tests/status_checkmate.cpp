#include <cassert>
#include "chess/position.hpp"
#include "chess/status.hpp"
#include "chess/fen.hpp"

// Classic Fool's Mate position (White to move, checkmated)
int main() {
    Position p;
    bool ok = loadFEN(p, "rnb1kbnr/pppp1ppp/8/4p3/6Pq/5P2/PPPPP2P/RNBQKBNR w KQkq - 0 3");
    assert(ok);
    GameStatus s = assessStatus(p);
    assert(s.phase == Phase::GameOver);
    assert(s.outcome == Outcome::Blackwins);
    assert(s.in_check == true);
    return 0;
}
