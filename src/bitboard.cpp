#include "chess/bitboard.hpp"

void print_board(Bitboard board)
{
    for (int i = 7; i >= 0; i--)
    {
        std::cout << i + 1 << "   ";
        for (int j = 0; j < 8; j++)
        {
            int index = i * 8 + j;
            if (is_Piece(board, index))
            {
                std::cout << "1 ";
            }
            else
            {
                std::cout << ". ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << "    a b c d e f g h\n";
}
