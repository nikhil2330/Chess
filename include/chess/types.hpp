#pragma once
#include <cstdint>

using Bitboard = std::uint64_t;

enum Color : std::uint8_t
{
    WHITE = 0,
    BLACK = 1
};

enum class Phase : uint8_t
{
    Playing,
    GameOver
};
enum class Outcome : uint8_t
{
    None,
    Whitewins,
    Blackwins,
    Draw
};
enum class DrawReason : uint8_t
{
    None,
    Stalemate,
    InsufficientMaterial,
    DeadPosition,
    FiftyMove,
    Threefold,
    Agreement
};

enum Castling : std::uint8_t
{
    White_King = 1,
    White_Queen = 2,
    Black_King = 4,
    Black_Queen = 8
};

enum MoveFlag : uint16_t
{
    CAPTURE = 1,
    DOUBLE_PUSH = 2,
    EN_PASSANT = 4,
    PROMOTION = 8,
};

enum Piece : uint8_t
{
    EMPTY = 0,
    WP,
    WN,
    WB,
    WR,
    WQ,
    WK,
    BP,
    BN,
    BB,
    BR,
    BQ,
    BK
};

enum Promo : uint8_t
{
    NO_PROMO = 0,
    PROMO_N = 1,
    PROMO_B = 2,
    PROMO_R = 3,
    PROMO_Q = 4
};
