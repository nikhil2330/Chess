#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>
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

static std::string move_to_uci_dbg(const Move& m) {
    // Re-uses your move_to_uci — but we’re in tests, so paste a tiny local version:
    auto sq_to_str = [](int idx){
        if (idx < 0 || idx > 63) return std::string("--");
        char f = 'a' + (idx % 8);
        char r = '1' + (idx / 8);
        return std::string{f, r};
    };
    auto promo_char = [](uint8_t promo)->char{
        switch (promo){
            case PROMO_Q: return 'q';
            case PROMO_R: return 'r';
            case PROMO_B: return 'b';
            case PROMO_N: return 'n';
            default: return 0;
        }
    };

    std::string s;
    s += sq_to_str(m.from);
    s += sq_to_str(m.to);
    if (m.flags & PROMOTION) s.push_back(promo_char(m.promo));
    return s;
}

int main() {
    // Focus the failing FEN (Position 4 in your perft suite)
    const char* FEN = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";

    Position p;
    bool ok = loadFEN(p, FEN);
    if (!ok) { std::cerr << "loadFEN failed\n"; return 1; }

    // Depth to divide (1 or 2). Start with 1 to catch the root mismatch.
    const int depth = 1;

    std::vector<Move> root;
    generateLegalAllMoves(p, root);

    std::cout << "FEN: " << FEN << "\n";
    std::cout << "Side: " << (p.side_to_move == WHITE ? "w" : "b") << "\n";
    std::cout << "Root legal moves: " << root.size() << "\n";

    // Classify to spot patterns (captures, EP, promotions, castling-like)
    int caps=0, eps=0, promos=0, doubles=0, castles=0;

    // Sort moves for stable output (by UCI)
    std::vector<std::pair<std::string, Move>> labeled;
    labeled.reserve(root.size());
    for (const auto& m : root) labeled.push_back({move_to_uci_dbg(m), m});
    std::sort(labeled.begin(), labeled.end(),
              [](auto& a, auto& b){ return a.first < b.first; });

    for (auto& lm : labeled) {
        const Move& m = lm.second;
        uint64_t nodes = 0;
        if (depth == 1) {
            nodes = 1; // leaf
        } else {
            makeMove(p, m);
            nodes = perft(p, depth - 1);
            UndoMove(p);
        }

        bool is_castle_like = false;
        // Castle detection (your engine moves rook implicitly if king moved two squares)
        // from/to diff of 2 file steps implies castling.
        if (std::abs((m.to % 8) - (m.from % 8)) == 2) is_castle_like = true;

        if (m.flags & CAPTURE)   ++caps;
        if (m.flags & EN_PASSANT)++eps;
        if (m.flags & PROMOTION) ++promos;
        if (m.flags & DOUBLE_PUSH) ++doubles;
        if (is_castle_like) ++castles;

        std::cout << lm.first
                  << "  nodes=" << nodes
                  << "  flags[" 
                  << ((m.flags & CAPTURE) ? "C" : "-")
                  << ((m.flags & EN_PASSANT) ? "E" : "-")
                  << ((m.flags & PROMOTION) ? "P" : "-")
                  << ((m.flags & DOUBLE_PUSH) ? "D" : "-")
                  << (is_castle_like ? "K" : "-")
                  << "]\n";
    }

    std::cout << "\nSummary @ depth " << depth << ":\n"
              << "  total=" << root.size() << "\n"
              << "  captures=" << caps
              << "  ep=" << eps
              << "  promotions=" << promos
              << "  double_push=" << doubles
              << "  castles_like=" << castles << "\n";

    // Expected total at depth=1 for this FEN is 6.
    std::cout << "\nNOTE: Expected perft(1) == 6 for this position.\n";
    return 0;
}
