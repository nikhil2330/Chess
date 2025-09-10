#pragma once
#include "chess/types.hpp"
#include "chess/position.hpp"

struct GameStatus
{
    Phase phase = Phase::Playing;
    Outcome outcome = Outcome::None;
    DrawReason draw_reason = DrawReason::None;
    bool in_check = false;
    Color side_to_move;
};

class Move;
#include "chess/move.hpp"
#include "chess/movegen.hpp"
#include "chess/attacks.hpp"
#include "chess/bitboard.hpp"

GameStatus assessStatus(Position &pos);
