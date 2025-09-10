#include "chess/zobrist.hpp"
#include "chess/position.hpp"   
#include "chess/bitboard.hpp"  

namespace Zobrist {

std::uint64_t PIECE_SQ[12][64];
std::uint64_t CASTLING[4];
std::uint64_t EP_FILE[8];
std::uint64_t SIDE;

static inline std::uint64_t splitmix64(std::uint64_t& s) {
    std::uint64_t z = (s += 0x9E3779B97F4A7C15ull);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
    return z ^ (z >> 31);
}

void init() {
    std::uint64_t seed = 0xC0FFEEULL ^ 0xFEEDBEEFULL; 
    for (int p = 0; p < 12; ++p)
        for (int sq = 0; sq < 64; ++sq)
            PIECE_SQ[p][sq] = splitmix64(seed);

    for (int i = 0; i < 4; ++i)   CASTLING[i] = splitmix64(seed);
    for (int f = 0; f < 8; ++f)   EP_FILE[f]  = splitmix64(seed);
    SIDE = splitmix64(seed);
}

std::uint64_t compute(const Position& pos) {
    std::uint64_t h = 0;

    auto mix_bb = [&](Bitboard bb, int pi) {
        Bitboard x = bb;
        while (x) {
            int sq = pop_lsb(x);
            h ^= PIECE_SQ[pi][sq];
        }
    };

    mix_bb(pos.P, piece_index(WP));
    mix_bb(pos.N, piece_index(WN));
    mix_bb(pos.B, piece_index(WB));
    mix_bb(pos.R, piece_index(WR));
    mix_bb(pos.Q, piece_index(WQ));
    mix_bb(pos.K, piece_index(WK));

    mix_bb(pos.p, piece_index(BP));
    mix_bb(pos.n, piece_index(BN));
    mix_bb(pos.b, piece_index(BB));
    mix_bb(pos.r, piece_index(BR));
    mix_bb(pos.q, piece_index(BQ));
    mix_bb(pos.k, piece_index(BK));

    if (pos.castling & White_King)  h ^= CASTLING[0];
    if (pos.castling & White_Queen) h ^= CASTLING[1];
    if (pos.castling & Black_King)  h ^= CASTLING[2];
    if (pos.castling & Black_Queen) h ^= CASTLING[3];

    if (pos.en_passant != -1)
        h ^= EP_FILE[pos.en_passant % 8];

    if (pos.side_to_move == BLACK)
        h ^= SIDE;

    return h;
}
}
