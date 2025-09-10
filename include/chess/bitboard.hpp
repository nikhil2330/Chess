#pragma once
#include <cstdint>
#include <limits>
#include <iostream>
#include "chess/types.hpp"

constexpr Bitboard FILE_A = 0x0101010101010101ULL;
constexpr Bitboard FILE_H = 0x8080808080808080ULL;
constexpr Bitboard FILE_B = 0x0202020202020202ULL;
constexpr Bitboard FILE_G = 0x4040404040404040ULL;

constexpr Bitboard RANK_1 = 0x00000000000000FFULL;
constexpr Bitboard RANK_2 = 0x000000000000FF00ULL;
constexpr Bitboard RANK_7 = 0x00FF000000000000ULL;
constexpr Bitboard RANK_8 = 0xFF00000000000000ULL;
constexpr Bitboard RANK_3 = 0x0000000000FF0000ULL;
constexpr Bitboard RANK_6 = 0x0000FF0000000000ULL;

constexpr Bitboard DARK_SQUARES = 0xAA55AA55AA55AA55ULL;
constexpr Bitboard LIGHT_SQUARES = 0x55AA55AA55AA55AAULL;

inline int get_index(char fileN, int rankN)
{
    int file = fileN - 'a';
    int rank = rankN - 1;
    return file + rank * 8;
}

inline Bitboard convert_to_bit(int index) { return 1ULL << index; }
inline bool is_Piece(Bitboard board, int index) { return (board & convert_to_bit(index)) != 0; }
inline Bitboard set_bit(Bitboard board, int index) { return board | convert_to_bit(index); }
inline Bitboard clear_bit(Bitboard board, int index) { return board & ~convert_to_bit(index); }
inline Bitboard toggle_bit(Bitboard board, int index) { return board ^ convert_to_bit(index); }

inline int bits_set_count(Bitboard board)
{
    int count = 0;
    while (board != 0)
    {
        board &= (board - 1);
        count++;
    }
    return count;
}
inline int peek_lsb(Bitboard board)
{
    if (board == 0)
        return -1;
    int x = 0;
    while ((board & 1ULL) == 0ULL)
    {
        board >>= 1;
        x++;
    }
    return x;
}
inline int pop_lsb(Bitboard &board)
{
    if (board == 0)
        return -1;
    int x = peek_lsb(board);
    board &= (board - 1);
    return x;
}

inline Bitboard shift_north(Bitboard board)
{
    return board << 8;
}
inline Bitboard shift_south(Bitboard board)
{
    return board >> 8;
}
inline Bitboard shift_east(Bitboard board)
{
    return (board & ~FILE_H) << 1;
}
inline Bitboard shift_west(Bitboard board)
{
    return (board & ~FILE_A) >> 1;
}
inline Bitboard shift_northeast(Bitboard board)
{
    return shift_east(shift_north(board));
}
inline Bitboard shift_northwest(Bitboard board)
{
    return shift_west(shift_north(board));
}
inline Bitboard shift_southeast(Bitboard board)
{
    return shift_east(shift_south(board));
}
inline Bitboard shift_southwest(Bitboard board)
{
    return shift_west(shift_south(board));
}

void print_board(Bitboard board);
