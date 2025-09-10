#pragma once
#include <stack>
#include "chess/types.hpp"
#include "chess/move.hpp"
#include "chess/position.hpp"

// From your code (unchanged)
struct MoveHistory
{
    Move move;
    Piece moved_piece;
    Piece captured_piece;
    int captured;
    int previous_en_passant;
    uint8_t prev_castling;
    int prev_halfmove;
    int prev_fullmove;
};

extern std::stack<MoveHistory> history;

void makeMove(Position &pos, const Move &move);
void UndoMove(Position &pos);
