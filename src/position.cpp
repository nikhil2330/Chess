#include "chess/position.hpp"
#include <cassert>
#include "chess/zobrist.hpp"


Piece getPiece(const Position &pos, int sq)
{
    Bitboard board = convert_to_bit(sq);
    if (pos.P & board)
        return WP;
    if (pos.N & board)
        return WN;
    if (pos.B & board)
        return WB;
    if (pos.R & board)
        return WR;
    if (pos.Q & board)
        return WQ;
    if (pos.K & board)
        return WK;

    if (pos.p & board)
        return BP;
    if (pos.n & board)
        return BN;
    if (pos.b & board)
        return BB;
    if (pos.r & board)
        return BR;
    if (pos.q & board)
        return BQ;
    if (pos.k & board)
        return BK;

    return EMPTY;
}

Piece removePiece(Position &pos, int sq)
{
    Bitboard board = convert_to_bit(sq);
    if (pos.P & board)
    {
        pos.P &= ~board;
        pos.white_pieces &= ~board;
        pos.total_pieces = pos.white_pieces | pos.black_pieces;
        return WP;
    }
    if (pos.N & board)
    {
        pos.N &= ~board;
        pos.white_pieces &= ~board;
        pos.total_pieces = pos.white_pieces | pos.black_pieces;
        return WN;
    }
    if (pos.B & board)
    {
        pos.B &= ~board;
        pos.white_pieces &= ~board;
        pos.total_pieces = pos.white_pieces | pos.black_pieces;
        return WB;
    }
    if (pos.R & board)
    {
        pos.R &= ~board;
        pos.white_pieces &= ~board;
        pos.total_pieces = pos.white_pieces | pos.black_pieces;
        return WR;
    }
    if (pos.Q & board)
    {
        pos.Q &= ~board;
        pos.white_pieces &= ~board;
        pos.total_pieces = pos.white_pieces | pos.black_pieces;
        return WQ;
    }
    if (pos.K & board)
    {
        pos.K &= ~board;
        pos.white_pieces &= ~board;
        pos.total_pieces = pos.white_pieces | pos.black_pieces;
        return WK;
    }

    if (pos.p & board)
    {
        pos.p &= ~board;
        pos.black_pieces &= ~board;
        pos.total_pieces = pos.white_pieces | pos.black_pieces;
        return BP;
    }
    if (pos.n & board)
    {
        pos.n &= ~board;
        pos.black_pieces &= ~board;
        pos.total_pieces = pos.white_pieces | pos.black_pieces;
        return BN;
    }
    if (pos.b & board)
    {
        pos.b &= ~board;
        pos.black_pieces &= ~board;
        pos.total_pieces = pos.white_pieces | pos.black_pieces;
        return BB;
    }
    if (pos.r & board)
    {
        pos.r &= ~board;
        pos.black_pieces &= ~board;
        pos.total_pieces = pos.white_pieces | pos.black_pieces;
        return BR;
    }
    if (pos.q & board)
    {
        pos.q &= ~board;
        pos.black_pieces &= ~board;
        pos.total_pieces = pos.white_pieces | pos.black_pieces;
        return BQ;
    }
    if (pos.k & board)
    {
        pos.k &= ~board;
        pos.black_pieces &= ~board;
        pos.total_pieces = pos.white_pieces | pos.black_pieces;
        return BK;
    }

    return EMPTY;
}

void addPiece(Position &pos, int sq, Piece p)
{
    Bitboard board = convert_to_bit(sq);

    assert(p != EMPTY && "add_piece: cannot add EMPTY");
    assert((pos.total_pieces & board) == 0 && "add_piece: square not empty");
    switch (p)
    {
    case WP:
        pos.P |= board;
        pos.white_pieces |= board;
        break;
    case WN:
        pos.N |= board;
        pos.white_pieces |= board;
        break;
    case WB:
        pos.B |= board;
        pos.white_pieces |= board;
        break;
    case WR:
        pos.R |= board;
        pos.white_pieces |= board;
        break;
    case WQ:
        pos.Q |= board;
        pos.white_pieces |= board;
        break;
    case WK:
        pos.K |= board;
        pos.white_pieces |= board;
        break;

    case BP:
        pos.p |= board;
        pos.black_pieces |= board;
        break;
    case BN:
        pos.n |= board;
        pos.black_pieces |= board;
        break;
    case BB:
        pos.b |= board;
        pos.black_pieces |= board;
        break;
    case BR:
        pos.r |= board;
        pos.black_pieces |= board;
        break;
    case BQ:
        pos.q |= board;
        pos.black_pieces |= board;
        break;
    case BK:
        pos.k |= board;
        pos.black_pieces |= board;
        break;

    default:
        assert(false && "add_piece: unknown piece id");
        break;
    }
    pos.total_pieces = pos.white_pieces | pos.black_pieces;
}

void print_pos_board(const Position &pos)
{
    std::cout << std::endl;
    for (int i = 7; i >= 0; i--)
    {
        std::cout << i + 1 << "   ";
        for (int j = 0; j < 8; j++)
        {
            int index = i * 8 + j;
            if (is_Piece(pos.P, index))
            {
                std::cout << "P ";
            }
            else if (is_Piece(pos.N, index))
            {
                std::cout << "N ";
            }
            else if (is_Piece(pos.B, index))
            {
                std::cout << "B ";
            }
            else if (is_Piece(pos.R, index))
            {
                std::cout << "R ";
            }
            else if (is_Piece(pos.Q, index))
            {
                std::cout << "Q ";
            }
            else if (is_Piece(pos.K, index))
            {
                std::cout << "K ";
            }
            else if (is_Piece(pos.p, index))
            {
                std::cout << "p ";
            }
            else if (is_Piece(pos.n, index))
            {
                std::cout << "n ";
            }
            else if (is_Piece(pos.b, index))
            {
                std::cout << "b ";
            }
            else if (is_Piece(pos.r, index))
            {
                std::cout << "r ";
            }
            else if (is_Piece(pos.q, index))
            {
                std::cout << "q ";
            }
            else if (is_Piece(pos.k, index))
            {
                std::cout << "k ";
            }
            else
            {
                std::cout << ". ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << "    a b c d e f g h\n\n";

    std::cout << "Side to move: " << (pos.side_to_move == WHITE ? "White" : "Black")
              << "  Castling: "
              << ((pos.castling & White_King) ? 'K' : '-')
              << ((pos.castling & White_Queen) ? 'Q' : '-')
              << ((pos.castling & Black_King) ? 'k' : '-')
              << ((pos.castling & Black_Queen) ? 'q' : '-')
              << "  En Passant: ";
    if (pos.en_passant == -1)
    {
        std::cout << "-";
    }
    else
    {
        char file = 'a' + (pos.en_passant % 8);
        char rank = '1' + (pos.en_passant / 8);
        std::cout << file << rank;
    }
    std::cout << "  Halfmove: " << pos.halfmove
              << "  Fullmove: " << pos.fullmove << "\n\n";
}
