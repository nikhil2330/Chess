#include <cassert>
#include <cstdint>
#include <vector>
#include <string>
#include "chess/position.hpp"
#include "chess/move.hpp"
#include "chess/movegen.hpp"
#include "chess/make_undo.hpp"
#include "chess/fen.hpp"

static uint64_t perft(Position& pos, int depth) {
    std::vector<Move> moves;
    generateLegalAllMoves(pos, moves);
    if (depth == 1) return static_cast<uint64_t>(moves.size());

    uint64_t nodes = 0;
    for (const auto& m : moves) {
        makeMove(pos, m);
        nodes += perft(pos, depth - 1);
        UndoMove(pos);
    }
    return nodes;
}

int main() {
    // ---- Position 1: Start position
    {
        Position p; p.start_position();
        assert(perft(p, 1) == 20ULL);
        assert(perft(p, 2) == 400ULL);
        assert(perft(p, 3) == 8902ULL);
        assert(perft(p, 4) == 197281ULL);
        // Depth 5 is 4,865,609 (can be slow in Debug). Uncomment if you want:
        assert(perft(p, 5) == 4865609ULL);
    }

    // ---- Position 2: Kiwipete
    // r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1
    {
        Position p;
        bool ok = loadFEN(p, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
        assert(ok);
        assert(perft(p, 1) == 48ULL);
        assert(perft(p, 2) == 2039ULL);
        assert(perft(p, 3) == 97862ULL);
        assert(perft(p, 4) == 4085603ULL);
        // Depth 5: 193690690 (very heavy) — skip in unit test.
    }

    // ---- Position 3: En passant tricky
    // 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1
    {
        Position p;
        bool ok = loadFEN(p, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
        assert(ok);
        assert(perft(p, 1) == 14ULL);
        assert(perft(p, 2) == 191ULL);
        assert(perft(p, 3) == 2812ULL);
        assert(perft(p, 4) == 43238ULL);
        assert(perft(p, 5) == 674624ULL);
    }

    // ---- Position 4: Promotions, castling, EP soup
    // r3k2r/Pppp1ppp/1b3nbN/nP6/BBPpP3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1
    {
        Position p;
        bool ok = loadFEN(p, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
        assert(ok);
        assert(perft(p, 1) == 6ULL);
        assert(perft(p, 2) == 264ULL);
        assert(perft(p, 3) == 9467ULL);
        assert(perft(p, 4) == 422333ULL);
        // Depth 5: 15833292 — large.
    }

    // ---- Position 5: Discovered checks, pins
    // rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8
    {
        Position p;
        bool ok = loadFEN(p, "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
        assert(ok);
        assert(perft(p, 1) == 44ULL);
        assert(perft(p, 2) == 1486ULL);
        assert(perft(p, 3) == 62379ULL);
        // Depth 4: 2103487 — larger, skip for speed.
    }

    // ---- Position 6: Bishops on both diags, tactics
    // r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10
    {
        Position p;
        bool ok = loadFEN(p, "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
        assert(ok);
        assert(perft(p, 1) == 46ULL);
        assert(perft(p, 2) == 2079ULL);
        assert(perft(p, 3) == 89890ULL);
        // Depth 4: 3894594 — big, often skipped.
    }

    return 0;
}
