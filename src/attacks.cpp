#include "chess/attacks.hpp"
#include "chess/move.hpp"
#include <array>

std::array<Bitboard, 64> generateKingMoveTable();
std::array<Bitboard, 64> generateKnightMoveTable();
std::array<std::array<Bitboard, 64>, 2> generatePawnCaptureTable();

const std::array<Bitboard, 64> KING_TABLE = generateKingMoveTable();
const std::array<Bitboard, 64> KNIGHT_TABLE = generateKnightMoveTable();
const std::array<std::array<Bitboard, 64>, 2> PAWN_CAPTURE_TABLE = generatePawnCaptureTable();

Bitboard computeRookMove(int sq, Bitboard curr)
{

    Bitboard temp = convert_to_bit(sq);
    Bitboard result = 0;
    while (true)
    {
        temp = shift_east(temp);
        if (temp == 0)
        {
            break;
        }
        result |= temp;
        int x = peek_lsb(temp);
        if (is_Piece(curr, x))
        {
            break;
        }
    }

    temp = convert_to_bit(sq);
    while (true)
    {
        temp = shift_west(temp);
        if (temp == 0)
        {
            break;
        }
        result |= temp;
        int x = peek_lsb(temp);
        if (is_Piece(curr, x))
        {
            break;
        }
    }

    temp = convert_to_bit(sq);
    while (true)
    {
        temp = shift_north(temp);
        if (temp == 0)
        {
            break;
        }
        result |= temp;
        int x = peek_lsb(temp);
        if (is_Piece(curr, x))
        {
            break;
        }
    }

    temp = convert_to_bit(sq);
    while (true)
    {
        temp = shift_south(temp);
        if (temp == 0)
        {
            break;
        }
        result |= temp;
        int x = peek_lsb(temp);
        if (is_Piece(curr, x))
        {
            break;
        }
    }

    return result;
}
Bitboard computeBishopMove(int sq, Bitboard curr)
{

    Bitboard temp = convert_to_bit(sq);
    Bitboard result = 0;
    while (true)
    {
        temp = shift_northeast(temp);
        if (temp == 0)
        {
            break;
        }
        result |= temp;
        int x = peek_lsb(temp);
        if (is_Piece(curr, x))
        {
            break;
        }
    }

    temp = convert_to_bit(sq);
    while (true)
    {
        temp = shift_northwest(temp);
        if (temp == 0)
        {
            break;
        }
        result |= temp;
        int x = peek_lsb(temp);
        if (is_Piece(curr, x))
        {
            break;
        }
    }

    temp = convert_to_bit(sq);
    while (true)
    {
        temp = shift_southeast(temp);
        if (temp == 0)
        {
            break;
        }
        result |= temp;
        int x = peek_lsb(temp);
        if (is_Piece(curr, x))
        {
            break;
        }
    }

    temp = convert_to_bit(sq);
    while (true)
    {
        temp = shift_southwest(temp);
        if (temp == 0)
        {
            break;
        }
        result |= temp;
        int x = peek_lsb(temp);
        if (is_Piece(curr, x))
        {
            break;
        }
    }
    return result;
}
Bitboard computeQueenMove(int sq, Bitboard curr)
{
    return computeBishopMove(sq, curr) | computeRookMove(sq, curr);
}

Bitboard computePawnCapture(Color col, int sq)
{
    return PAWN_CAPTURE_TABLE[col][sq];
}

bool isPieceAttacked(int sq, Color opp, const Position &pos)
{
    Bitboard sources = computePawnCapture((opp == WHITE) ? BLACK : WHITE, sq);
    Bitboard pieces = (opp == WHITE) ? pos.P : pos.p;
    if (sources & pieces)
    {
        return true;
    }
    if ((KNIGHT_TABLE[sq] & ((opp == WHITE) ? pos.N : pos.n)) != 0)
        return true;

    if ((KING_TABLE[sq] & ((opp == WHITE) ? pos.K : pos.k)) != 0)
        return true;

    sources = computeBishopMove(sq, pos.total_pieces);
    pieces = (opp == WHITE) ? (pos.B | pos.Q) : (pos.b | pos.q);
    if (sources & pieces)
        return true;

    sources = computeRookMove(sq, pos.total_pieces);
    pieces = (opp == WHITE) ? (pos.R | pos.Q) : (pos.r | pos.q);
    if (sources & pieces)
        return true;

    return false;
}

int kingSquare(Color side, const Position &pos)
{
    return (side == WHITE) ? peek_lsb(pos.K) : peek_lsb(pos.k);
}

bool isKinginCheck(Color side, const Position &pos)
{
    return isPieceAttacked(kingSquare(side, pos), (side == WHITE) ? BLACK : WHITE, pos);
}

std::array<Bitboard, 64> generateKingMoveTable()
{
    std::array<Bitboard, 64> table;
    for (int i = 0; i < 64; i++)
    {
        table[i] = shift_north(convert_to_bit(i)) |
                   shift_south(convert_to_bit(i)) |
                   shift_east(convert_to_bit(i)) |
                   shift_west(convert_to_bit(i)) |
                   shift_northeast(convert_to_bit(i)) |
                   shift_northwest(convert_to_bit(i)) |
                   shift_southeast(convert_to_bit(i)) |
                   shift_southwest(convert_to_bit(i));
    }
    return table;
}
std::array<Bitboard, 64> generateKnightMoveTable()
{
    std::array<Bitboard, 64> table;
    for (int i = 0; i < 64; i++)
    {
        table[i] = ((convert_to_bit(i) & ~FILE_A) << 15) |
                   ((convert_to_bit(i) & ~FILE_A) >> 17) |
                   ((convert_to_bit(i) & ~FILE_H) << 17) |
                   ((convert_to_bit(i) & ~FILE_H) >> 15) |
                   ((convert_to_bit(i) & ~(FILE_A | FILE_B)) << 6) |
                   ((convert_to_bit(i) & ~(FILE_A | FILE_B)) >> 10) |
                   ((convert_to_bit(i) & ~(FILE_G | FILE_H)) >> 6) |
                   ((convert_to_bit(i) & ~(FILE_G | FILE_H)) << 10);
    }
    return table;
}
std::array<std::array<Bitboard, 64>, 2> generatePawnCaptureTable()
{
    std::array<std::array<Bitboard, 64>, 2> table;
    for (int i = 0; i < 64; i++)
    {
        table[WHITE][i] = shift_northeast(convert_to_bit(i)) |
                          shift_northwest(convert_to_bit(i));
    }
    for (int i = 0; i < 64; i++)
    {
        table[BLACK][i] = shift_southeast(convert_to_bit(i)) |
                          shift_southwest(convert_to_bit(i));
    }
    return table;
}
