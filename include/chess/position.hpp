#pragma once
#include "chess/types.hpp"
#include "chess/bitboard.hpp"
#include "chess/zobrist.hpp"
#include "chess/repetition.hpp"



struct Position
{
    Bitboard P = 0, N = 0, B = 0, R = 0, Q = 0, K = 0; // White
    Bitboard p = 0, n = 0, b = 0, r = 0, q = 0, k = 0; // Black

    Bitboard white_pieces = 0, black_pieces = 0, total_pieces = 0;
    Color side_to_move = WHITE;
    std::uint8_t castling = 0;
    std::uint64_t zobrist = 0;
    int en_passant = -1;
    int halfmove = 0;
    int fullmove = 0;
    void board_state()
    {
        white_pieces = P | N | B | R | Q | K;
        black_pieces = p | n | b | r | q | k;
        total_pieces = white_pieces | black_pieces;
    }
    void clear()
    {
        P = N = B = R = Q = K = 0;
        p = n = b = r = q = k = 0;
        white_pieces = black_pieces = total_pieces = 0;
        side_to_move = WHITE;
        castling = 0;
        en_passant = -1;
        halfmove = 0;
        fullmove = 0;
    }

    void start_position()
    {
        clear();
        P = RANK_2;
        p = RANK_7;

        R = set_bit(R, get_index('a', 1)) | set_bit(R, get_index('h', 1));
        r = set_bit(r, get_index('a', 8)) | set_bit(r, get_index('h', 8));

        B = set_bit(B, get_index('c', 1)) | set_bit(B, get_index('f', 1));
        b = set_bit(b, get_index('c', 8)) | set_bit(b, get_index('f', 8));

        N = set_bit(N, get_index('b', 1)) | set_bit(N, get_index('g', 1));
        n = set_bit(n, get_index('b', 8)) | set_bit(n, get_index('g', 8));

        K = set_bit(K, get_index('e', 1));
        k = set_bit(k, get_index('e', 8));

        Q = set_bit(Q, get_index('d', 1));
        q = set_bit(q, get_index('d', 8));

        side_to_move = WHITE;
        castling = White_King | White_Queen | Black_King | Black_Queen;
        en_passant = -1;
        halfmove = 0;
        fullmove = 1;

        board_state();
        Zobrist::init();
        zobrist = Zobrist::compute(*this);
        rep_init(*this);

    }
};

inline Bitboard ally_piece(const Position &pos, Color col) { return (col == WHITE) ? pos.white_pieces : pos.black_pieces; }
inline Bitboard opp_piece(const Position &pos, Color col) { return (col == WHITE) ? pos.black_pieces : pos.white_pieces; }

Piece getPiece(const Position &pos, int sq);
Piece removePiece(Position &pos, int sq);
void addPiece(Position &pos, int sq, Piece p);
void print_pos_board(const Position &pos);
