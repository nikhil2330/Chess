#include <iostream>
#include "chess/position.hpp"
#include "chess/fen.hpp"


int main()
{
    Position p;
    bool ok = loadFEN(p, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    print_pos_board(p);
    return 0;
}
