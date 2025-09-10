#pragma once
#include <array>
#include "chess/types.hpp"
#include "chess/bitboard.hpp"
#include "chess/position.hpp"

// From your code (unchanged signatures)
std::array<Bitboard, 64> generateKingMoveTable();
extern const std::array<Bitboard, 64> KING_TABLE;

std::array<Bitboard, 64> generateKnightMoveTable();
extern const std::array<Bitboard, 64> KNIGHT_TABLE;

std::array<std::array<Bitboard, 64>, 2> generatePawnCaptureTable();
extern const std::array<std::array<Bitboard, 64>, 2> PAWN_CAPTURE_TABLE;

// Sliding and derived attacks (unchanged)
Bitboard computeRookMove(int sq, Bitboard curr);
Bitboard computeBishopMove(int sq, Bitboard curr);
Bitboard computeQueenMove(int sq, Bitboard curr);

// Inline wrappers (unchanged)
inline Bitboard computeKnightMoves(Color side, const Position &pos, int sq)
{
    return KNIGHT_TABLE[sq] & ~ally_piece(pos, side);
}
inline Bitboard computeKingMoves(Color side, const Position &pos, int sq)
{
    return KING_TABLE[sq] & ~ally_piece(pos, side);
}
inline Bitboard computeBishopMoves(Color side, const Position &pos, int sq)
{
    return computeBishopMove(sq, pos.total_pieces) & ~ally_piece(pos, side);
}
inline Bitboard computeRookMoves(Color side, const Position &pos, int sq)
{
    return computeRookMove(sq, pos.total_pieces) & ~ally_piece(pos, side);
}
inline Bitboard computeQueenMoves(Color side, const Position &pos, int sq)
{
    return computeQueenMove(sq, pos.total_pieces) & ~ally_piece(pos, side);
}

Bitboard computePawnCapture(Color col, int sq);
bool isPieceAttacked(int sq, Color opp, const Position &pos);
int kingSquare(Color side, const Position &pos);
bool isKinginCheck(Color side, const Position &pos);
