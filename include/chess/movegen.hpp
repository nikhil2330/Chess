#pragma once
#include <vector>
#include "chess/types.hpp"
#include "chess/move.hpp"
#include "chess/position.hpp"
#include "chess/bitboard.hpp"
#include "chess/attacks.hpp"

// Inline push/capture helpers (unchanged)
inline Bitboard PawnSinglePushTo(Color col, const Position &pos, int sq)
{
    Bitboard temp = convert_to_bit(sq);
    if (col == WHITE)
    {
        Bitboard result = shift_north(temp);
        return (result & ~pos.total_pieces) & ~RANK_8;
    }
    else
    {
        Bitboard result = shift_south(temp);
        return (result & ~pos.total_pieces) & ~RANK_1;
    }
}

inline Bitboard PawnDoublePushTo(Color col, const Position &pos, int sq)
{
    Bitboard temp = convert_to_bit(sq);
    if (col == WHITE)
    {
        if ((temp & RANK_2) == 0)
            return 0;
        Bitboard one = shift_north(temp) & ~pos.total_pieces;
        if (!one)
            return 0;
        Bitboard two = shift_north(one) & ~pos.total_pieces;
        return two;
    }
    else
    {
        if ((temp & RANK_7) == 0)
            return 0;
        Bitboard one = shift_south(temp) & ~pos.total_pieces;
        if (!one)
            return 0;
        Bitboard two = shift_south(one) & ~pos.total_pieces;
        return two;
    }
}

inline Bitboard PawnPromoPushTo(Color col, const Position &pos, int sq)
{
    Bitboard temp = convert_to_bit(sq);
    if (col == WHITE)
    {
        Bitboard result = shift_north(temp) & ~pos.total_pieces;
        return result & RANK_8;
    }
    else
    {
        Bitboard result = shift_south(temp) & ~pos.total_pieces;
        return result & RANK_1;
    }
}

inline Bitboard PawnCapturesNormalTo(Color col, const Position &pos, int sq)
{
    Bitboard board = computePawnCapture(col, sq) & opp_piece(pos, col);
    return (col == WHITE) ? (board & ~RANK_8) : (board & ~RANK_1);
}
inline Bitboard PawnCapturesPromotionTo(Color col, const Position &pos, int sq)
{
    Bitboard board = computePawnCapture(col, sq) & opp_piece(pos, col);
    return (col == WHITE) ? (board & RANK_8) : (board & RANK_1);
}

// Non-inline list helpers (from your code, defined in src/movegen.cpp)
Bitboard PawnSinglePushToList(Color col, Position &pos);
Bitboard PawnDoublePushToList(Color col, Position &pos);
Bitboard PawnPromoPushToList(Color col, Position &pos);
Bitboard PawnCapturesNormalToList(Color col, Position &pos);
Bitboard PawnCapturesPromotionToList(Color col, Position &pos);
Bitboard enPassantFromList(Color col, Position &pos);
Bitboard enPassantFrom(Color col, const Position &pos, int sq);

// Move generators (unchanged)
void generatePawnMoves(const Position &pos, std::vector<Move> &list);
void generateKnightMoves(const Position &pos, std::vector<Move> &list);
void generateKingMoves(const Position &pos, std::vector<Move> &list);
void generateBishopMoves(const Position &pos, std::vector<Move> &list);
void generateRookMoves(const Position &pos, std::vector<Move> &list);
void generateQueenMoves(const Position &pos, std::vector<Move> &list);
void generateAllMoves(const Position &pos, std::vector<Move> &list);
void generateLegalAllMoves(Position &pos, std::vector<Move> &final);
