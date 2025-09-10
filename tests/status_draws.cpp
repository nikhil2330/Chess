#include <cassert>
#include <string>
#include "chess/position.hpp"
#include "chess/status.hpp"
#include "chess/fen.hpp"

int main() {
    // Insufficient material (K vs K) → draw
    Position p1;
    bool ok1 = loadFEN(p1, "8/8/8/8/8/8/8/4K2k w - - 0 1");
    assert(ok1);
    GameStatus s1 = assessStatus(p1);
    assert(s1.phase == Phase::GameOver);
    assert(s1.outcome == Outcome::Draw);
    assert(s1.draw_reason == DrawReason::InsufficientMaterial);

    // Fifty-move rule: halfmove >= 100 → draw
    Position p2; p2.start_position();
    p2.halfmove = 100;
    GameStatus s2 = assessStatus(p2);
    assert(s2.phase == Phase::GameOver);
    assert(s2.outcome == Outcome::Draw);
    assert(s2.draw_reason == DrawReason::FiftyMove);

    return 0;
}
