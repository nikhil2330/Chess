#include "chess/make_undo.hpp"
#include "chess/attacks.hpp"
#include <cassert>
#include "chess/zobrist.hpp"
#include "chess/repetition.hpp"
#include <vector>

std::stack<MoveHistory> history;

void makeMove(Position &pos, const Move &move)
{
    MoveHistory hist;
    hist.move = move;
    hist.moved_piece = getPiece(pos, move.from);
    hist.captured = -1;
    hist.captured_piece = EMPTY;
    hist.prev_castling = pos.castling;
    hist.prev_fullmove = pos.fullmove;
    hist.prev_halfmove = pos.halfmove;
    hist.previous_en_passant = pos.en_passant;

    if (pos.en_passant != -1)
    {
        pos.zobrist ^= Zobrist::EP_FILE[pos.en_passant % 8];
    }

    pos.en_passant = -1;
    {
        int x = Zobrist::piece_index(hist.moved_piece);
        pos.zobrist ^= Zobrist::PIECE_SQ[x][move.from];
    }

    removePiece(pos, move.from);
    if (move.flags & EN_PASSANT)
    {
        hist.captured = (pos.side_to_move == WHITE) ? (move.to - 8) : (move.to + 8);
        hist.captured_piece = (pos.side_to_move == WHITE) ? BP : WP;

        {
            int h = Zobrist::piece_index(hist.captured_piece);
            pos.zobrist ^= Zobrist::PIECE_SQ[h][hist.captured];
        }

        removePiece(pos, hist.captured);
    }
    else if (move.flags & CAPTURE)
    {
        hist.captured_piece = getPiece(pos, move.to);
        hist.captured = move.to;

        {
            int h = Zobrist::piece_index(hist.captured_piece);
            pos.zobrist ^= Zobrist::PIECE_SQ[h][move.to];
        }

        removePiece(pos, move.to);
    }
    if (move.flags & PROMOTION)
    {
        Piece promo;
        if (move.promo == PROMO_N)
        {
            promo = pos.side_to_move == WHITE ? WN : BN;
        }
        if (move.promo == PROMO_B)
        {
            promo = pos.side_to_move == WHITE ? WB : BB;
        }
        if (move.promo == PROMO_R)
        {
            promo = pos.side_to_move == WHITE ? WR : BR;
        }
        if (move.promo == PROMO_Q)
        {
            promo = pos.side_to_move == WHITE ? WQ : BQ;
        }
        addPiece(pos, move.to, promo);

        {
            int h = Zobrist::piece_index(promo);
            pos.zobrist ^= Zobrist::PIECE_SQ[h][move.to];
        }
    }
    else
    {
        addPiece(pos, move.to, hist.moved_piece);
        {
            int h = Zobrist::piece_index(hist.moved_piece);
            pos.zobrist ^= Zobrist::PIECE_SQ[h][move.to];
        }
    }

    if ((hist.moved_piece == WK || hist.moved_piece == BK) && std::abs(move.from - move.to) == 2)
    {
        if (pos.side_to_move == WHITE)
        {
            if (move.to == get_index('g', 1))
            {
                {
                    int h = Zobrist::piece_index(WR);
                    pos.zobrist ^= Zobrist::PIECE_SQ[h][get_index('h', 1)];
                    pos.zobrist ^= Zobrist::PIECE_SQ[h][get_index('f', 1)];
                }
                removePiece(pos, get_index('h', 1));
                addPiece(pos, get_index('f', 1), WR);
            }
            else
            {
                {
                    int h = Zobrist::piece_index(WR);
                    pos.zobrist ^= Zobrist::PIECE_SQ[h][get_index('a', 1)];
                    pos.zobrist ^= Zobrist::PIECE_SQ[h][get_index('d', 1)];
                }
                removePiece(pos, get_index('a', 1));
                addPiece(pos, get_index('d', 1), WR);
            }
        }
        else
        {
            if (move.to == get_index('g', 8))
            {
                {
                    int h = Zobrist::piece_index(BR);
                    pos.zobrist ^= Zobrist::PIECE_SQ[h][get_index('h', 8)];
                    pos.zobrist ^= Zobrist::PIECE_SQ[h][get_index('f', 8)];
                }
                removePiece(pos, get_index('h', 8));
                addPiece(pos, get_index('f', 8), BR);
            }
            else
            {
                {
                    int h = Zobrist::piece_index(BR);
                    pos.zobrist ^= Zobrist::PIECE_SQ[h][get_index('a', 8)];
                    pos.zobrist ^= Zobrist::PIECE_SQ[h][get_index('d', 8)];
                }
                removePiece(pos, get_index('a', 8));
                addPiece(pos, get_index('d', 8), BR);
            }
        }
    }

    if (hist.moved_piece == WK)
    {
        pos.castling &= ~(White_King | White_Queen);
    }
    else if (hist.moved_piece == BK)
    {
        pos.castling &= ~(Black_King | Black_Queen);
    }
    else if (hist.moved_piece == WR)
    {
        if (move.from == get_index('h', 1))
        {
            pos.castling &= ~White_King;
        }
        if (move.from == get_index('a', 1))
        {
            pos.castling &= ~White_Queen;
        }
    }
    else if (hist.moved_piece == BR)
    {
        if (move.from == get_index('h', 8))
        {
            pos.castling &= ~Black_King;
        }
        if (move.from == get_index('a', 8))
        {
            pos.castling &= ~Black_Queen;
        }
    }

    if (hist.captured_piece == WR)
    {
        if (hist.captured == get_index('h', 1))
        {
            pos.castling &= ~White_King;
        }
        if (hist.captured == get_index('a', 1))
        {
            pos.castling &= ~White_Queen;
        }
    }
    else if (hist.captured_piece == BR)
    {
        if (hist.captured == get_index('h', 8))
        {
            pos.castling &= ~Black_King;
        }
        if (hist.captured == get_index('a', 8))
        {
            pos.castling &= ~Black_Queen;
        }
    }

    if (move.flags & DOUBLE_PUSH)
    {
        if (hist.moved_piece == WP)
        {
            pos.en_passant = move.from + 8;
        }
        else if (hist.moved_piece == BP)
        {
            pos.en_passant = move.from - 8;
        }
    }

    bool pawnMove = (hist.moved_piece == WP || hist.moved_piece == BP);
    bool didCapture = (move.flags & (CAPTURE | EN_PASSANT)) != 0;
    if (pawnMove || didCapture || (move.flags & PROMOTION))
    {
        pos.halfmove = 0;
    }
    else
    {
        pos.halfmove += 1;
    }

    {
        std::uint8_t diff = hist.prev_castling ^ pos.castling;
        if (diff & White_King)  pos.zobrist ^= Zobrist::CASTLING[0];
        if (diff & White_Queen) pos.zobrist ^= Zobrist::CASTLING[1];
        if (diff & Black_King)  pos.zobrist ^= Zobrist::CASTLING[2];
        if (diff & Black_Queen) pos.zobrist ^= Zobrist::CASTLING[3];
    }

    if (pos.en_passant != -1) {
        pos.zobrist ^= Zobrist::EP_FILE[pos.en_passant % 8];
    }

    pos.side_to_move = (pos.side_to_move == WHITE) ? BLACK : WHITE;

    pos.zobrist ^= Zobrist::SIDE;

    rep_push(pos);


    if (pos.side_to_move == WHITE)
    {
        pos.fullmove += 1;
    }

    history.push(hist);
}

void UndoMove(Position &pos)
{
    assert(!history.empty());
    MoveHistory hist = history.top();
    history.pop();
    if ((hist.moved_piece == WK || hist.moved_piece == BK) && std::abs(hist.move.from - hist.move.to) == 2)
    {
        if (hist.moved_piece == WK)
        {
            if (hist.move.to == get_index('g', 1))
            {
                removePiece(pos, get_index('f', 1));
                addPiece(pos, get_index('h', 1), WR);
            }
            else
            {
                removePiece(pos, get_index('d', 1));
                addPiece(pos, get_index('a', 1), WR);
            }
        }
        else
        {
            if (hist.move.to == get_index('g', 8))
            {
                removePiece(pos, get_index('f', 8));
                addPiece(pos, get_index('h', 8), BR);
            }
            else
            {
                removePiece(pos, get_index('d', 8));
                addPiece(pos, get_index('a', 8), BR);
            }
        }
    }

    removePiece(pos, hist.move.to);
    if (hist.move.flags & PROMOTION)
    {
        addPiece(pos, hist.move.from, hist.moved_piece);
    }
    else
    {
        addPiece(pos, hist.move.from, hist.moved_piece);
    }

    if (hist.captured_piece != EMPTY)
    {
        addPiece(pos, hist.captured, hist.captured_piece);
    }
    pos.castling = hist.prev_castling;
    pos.en_passant = hist.previous_en_passant;
    pos.halfmove = hist.prev_halfmove;
    pos.fullmove = hist.prev_fullmove;
    pos.side_to_move = (pos.side_to_move == WHITE) ? BLACK : WHITE;

    pos.zobrist = Zobrist::compute(pos);
    rep_pop();
}
