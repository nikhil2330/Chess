#include "chess/status.hpp"
#include "chess/repetition.hpp"

GameStatus assessStatus(Position &pos)
{
    GameStatus s;
    std::vector<Move> temp;
    generateLegalAllMoves(pos, temp);
    s.side_to_move = pos.side_to_move;
    s.in_check = false;
    s.phase = Phase::Playing;
    s.outcome = Outcome::None;
    s.draw_reason = DrawReason::None;
    bool check = isKinginCheck(pos.side_to_move, pos);

    

    if (check)
    {
        s.in_check = true;
    }

    if(is_threefold(pos)){
        s.phase = Phase::GameOver;
        s.outcome = Outcome::Draw;
        s.draw_reason = DrawReason::Threefold;
        return s;
    }
    
    if (pos.halfmove >= 100)
    {
        s.phase = Phase::GameOver;
        s.outcome = Outcome::Draw;
        s.draw_reason = DrawReason::FiftyMove;
        return s;
    }
    if ((pos.P | pos.p | pos.R | pos.r | pos.Q | pos.q) == 0)
    {
        int N = bits_set_count(pos.N);
        int n = bits_set_count(pos.n);
        int B = bits_set_count(pos.B);
        int b = bits_set_count(pos.b);
        int Kk = bits_set_count(pos.total_pieces);
        bool z = Kk == 2;
        bool u = (B == 1) && (Kk == 3) || (b == 1) && (Kk == 3);
        bool v = (N == 1) && (Kk == 3) || (n == 1) && (Kk == 3);
        bool w = (N == 2) && (Kk == 4) || (n == 2) && (Kk == 4);
        bool x = (N == 1) && (Kk == 4) && (n == 1);
        bool y = (B == 1) && (Kk == 4) && (b == 1);
        bool t = (N == 1) && (Kk == 4) && (b == 1) || (n == 1) && (Kk == 4) && (B == 1);

        if (t || u || v || w || x || y || z)
        {
            s.phase = Phase::GameOver;
            s.outcome = Outcome::Draw;
            s.draw_reason = DrawReason::InsufficientMaterial;
            return s;
        }
    }

    if (check && temp.size() == 0)
    {
        s.in_check = true;
        s.phase = Phase::GameOver;
        s.outcome = pos.side_to_move == WHITE ? Outcome::Blackwins : Outcome::Whitewins;
        return s;
    }
    if (!check && temp.size() == 0)
    {
        s.phase = Phase::GameOver;
        s.outcome = Outcome::Draw;
        s.draw_reason = DrawReason::Stalemate;
        return s;
    }
    return s;
}
