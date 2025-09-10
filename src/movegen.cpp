#include "chess/movegen.hpp"
#include "chess/make_undo.hpp"
#include "chess/status.hpp"

Bitboard PawnSinglePushToList(Color col, Position &pos)
{
    if (col == WHITE)
    {
        return (shift_north(pos.P) & ~pos.total_pieces) & ~RANK_8;
    }
    else
    {
        return (shift_south(pos.p) & ~pos.total_pieces) & ~RANK_1;
    }
}

Bitboard PawnDoublePushToList(Color col, Position &pos)
{
    if (col == WHITE)
    {
        return shift_north(shift_north(pos.P & RANK_2) & ~pos.total_pieces) & ~pos.total_pieces;
    }
    else
    {
        return shift_south(shift_south(pos.p & RANK_7) & ~pos.total_pieces) & ~pos.total_pieces;
    }
}

Bitboard PawnPromoPushToList(Color col, Position &pos)
{
    if (col == WHITE)
    {
        return (shift_north(pos.P) & ~pos.total_pieces) & RANK_8;
    }
    else
    {
        return (shift_south(pos.p) & ~pos.total_pieces) & RANK_1;
    }
}

Bitboard PawnCapturesNormalToList(Color col, Position &pos)
{
    Bitboard opp = opp_piece(pos, col);
    return (col == WHITE)
               ? ((shift_northwest(pos.P) | shift_northeast(pos.P)) & opp & ~RANK_8)
               : ((shift_southwest(pos.p) | shift_southeast(pos.p)) & opp & ~RANK_1);
}

Bitboard PawnCapturesPromotionToList(Color col, Position &pos)
{
    Bitboard opp = opp_piece(pos, col);
    return (col == WHITE)
               ? ((shift_northwest(pos.P) | shift_northeast(pos.P)) & opp & RANK_8)
               : ((shift_southwest(pos.p) | shift_southeast(pos.p)) & opp & RANK_1);
}

Bitboard enPassantFromList(Color col, Position &pos)
{
    if (pos.en_passant == -1)
        return 0;
    if (col == WHITE)
    {
        return PAWN_CAPTURE_TABLE[BLACK][pos.en_passant] & pos.P;
    }
    else
    {
        return PAWN_CAPTURE_TABLE[WHITE][pos.en_passant] & pos.p;
    }
}

Bitboard enPassantFrom(Color col, const Position &pos, int sq)
{
    if (pos.en_passant == -1)
        return 0;
    return computePawnCapture(col, sq) & convert_to_bit(pos.en_passant);
}

void generatePawnMoves(const Position &pos, std::vector<Move> &list)
{
    Bitboard pawns = (pos.side_to_move == WHITE) ? pos.P : pos.p;
    while (pawns)
    {
        int from = pop_lsb(pawns);
        if (Bitboard single = PawnSinglePushTo(pos.side_to_move, pos, from))
        {
            int to = peek_lsb(single);
            list.push_back(Move{from, to, 0, NO_PROMO, -1});
        }

        if (Bitboard doub = PawnDoublePushTo(pos.side_to_move, pos, from))
        {
            int to = peek_lsb(doub);
            list.push_back({from, to, DOUBLE_PUSH, NO_PROMO, -1});
        }

        Bitboard promoPush = PawnPromoPushTo(pos.side_to_move, pos, from);
        while (promoPush)
        {
            int to = pop_lsb(promoPush);
            list.push_back({from, to, PROMOTION, PROMO_Q, -1});
            list.push_back({from, to, PROMOTION, PROMO_R, -1});
            list.push_back({from, to, PROMOTION, PROMO_B, -1});
            list.push_back({from, to, PROMOTION, PROMO_N, -1});
        }

        Bitboard captures = PawnCapturesNormalTo(pos.side_to_move, pos, from);
        while (captures)
        {
            int to = pop_lsb(captures);
            list.push_back({from, to, CAPTURE, NO_PROMO, -1});
        }

        Bitboard promoCaptures = PawnCapturesPromotionTo(pos.side_to_move, pos, from);
        while (promoCaptures)
        {
            int to = pop_lsb(promoCaptures);
            list.push_back({from, to, CAPTURE | PROMOTION, PROMO_Q, -1});
            list.push_back({from, to, CAPTURE | PROMOTION, PROMO_R, -1});
            list.push_back({from, to, CAPTURE | PROMOTION, PROMO_B, -1});
            list.push_back({from, to, CAPTURE | PROMOTION, PROMO_N, -1});
        }

        if (Bitboard enPassant = enPassantFrom(pos.side_to_move, pos, from))
        {
            int to = peek_lsb(enPassant);
            list.push_back({from, to, EN_PASSANT, NO_PROMO, -1});
        }
    }
}

void generateKnightMoves(const Position &pos, std::vector<Move> &list)
{
    Bitboard knights = (pos.side_to_move == WHITE) ? pos.N : pos.n;
    Bitboard opp = opp_piece(pos, pos.side_to_move);

    while (knights)
    {
        int from = pop_lsb(knights);
        Bitboard moves = computeKnightMoves(pos.side_to_move, pos, from);
        Bitboard captures = moves & opp;
        Bitboard normal = moves & ~pos.total_pieces;
        while (captures)
        {
            int to = pop_lsb(captures);
            list.push_back({from, to, CAPTURE, NO_PROMO, -1});
        }
        while (normal)
        {
            int to = pop_lsb(normal);
            list.push_back({from, to, 0, NO_PROMO, -1});
        }
    }
}

void generateKingMoves(const Position &pos, std::vector<Move> &list)
{
    Bitboard king = (pos.side_to_move == WHITE) ? pos.K : pos.k;
    Bitboard opp = opp_piece(pos, pos.side_to_move);
    int from = pop_lsb(king);
    Bitboard moves = computeKingMoves(pos.side_to_move, pos, from);
    Bitboard captures = moves & opp;
    Bitboard normal = moves & ~pos.total_pieces;
    while (captures)
    {
        int to = pop_lsb(captures);
        list.push_back({from, to, CAPTURE, NO_PROMO, -1});
    }
    while (normal)
    {
        int to = pop_lsb(normal);
        list.push_back({from, to, 0, NO_PROMO, -1});
    }
    Color op = pos.side_to_move == WHITE ? BLACK : WHITE;
    if (pos.side_to_move == WHITE)
    {
        int a1 = get_index('a', 1);
        int b1 = get_index('b', 1);
        int c1 = get_index('c', 1);
        int d1 = get_index('d', 1);
        int e1 = get_index('e', 1);
        int f1 = get_index('f', 1);
        int g1 = get_index('g', 1);
        int h1 = get_index('h', 1);
        if (pos.castling & White_King)
        {
            if ((getPiece(pos, f1) == EMPTY && getPiece(pos, g1) == EMPTY) &&
                (!isPieceAttacked(e1, op, pos) && !isPieceAttacked(f1, op, pos) &&
                 !isPieceAttacked(g1, op, pos)) &&
                is_Piece(pos.R, h1))
            {
                list.push_back({e1, g1, 0, NO_PROMO, -1});
            }
        }
        if (pos.castling & White_Queen)
        {
            if ((getPiece(pos, b1) == EMPTY && getPiece(pos, c1) == EMPTY &&
                 getPiece(pos, d1) == EMPTY) &&
                (!isPieceAttacked(e1, op, pos) &&
                 !isPieceAttacked(d1, op, pos) && !isPieceAttacked(c1, op, pos)) &&
                is_Piece(pos.R, a1))
            {
                list.push_back({e1, c1, 0, NO_PROMO, -1});
            }
        }
    }
    else
    {

        int a8 = get_index('a', 8);
        int b8 = get_index('b', 8);
        int c8 = get_index('c', 8);
        int d8 = get_index('d', 8);
        int e8 = get_index('e', 8);
        int f8 = get_index('f', 8);
        int g8 = get_index('g', 8);
        int h8 = get_index('h', 8);
        if (pos.castling & Black_King)
        {
            if ((getPiece(pos, f8) == EMPTY && getPiece(pos, g8) == EMPTY) &&
                (!isPieceAttacked(e8, op, pos) && !isPieceAttacked(f8, op, pos) &&
                 !isPieceAttacked(g8, op, pos)) &&
                is_Piece(pos.r, h8))
            {
                list.push_back({e8, g8, 0, NO_PROMO, -1});
            }
        }
        if (pos.castling & Black_Queen)
        {
            if ((getPiece(pos, b8) == EMPTY && getPiece(pos, c8) == EMPTY &&
                 getPiece(pos, d8) == EMPTY) &&
                (!isPieceAttacked(e8, op, pos) &&
                 !isPieceAttacked(d8, op, pos) && !isPieceAttacked(c8, op, pos)) &&
                is_Piece(pos.r, a8))
            {
                list.push_back({e8, c8, 0, NO_PROMO, -1});
            }
        }
    }
}

void generateBishopMoves(const Position &pos, std::vector<Move> &list)
{
    Bitboard bishops = (pos.side_to_move == WHITE) ? pos.B : pos.b;
    Bitboard opp = opp_piece(pos, pos.side_to_move);
    while (bishops)
    {
        int from = pop_lsb(bishops);
        Bitboard moves = computeBishopMoves(pos.side_to_move, pos, from);
        Bitboard captures = moves & opp;
        Bitboard normal = moves & ~pos.total_pieces;
        while (captures)
        {
            int to = pop_lsb(captures);
            list.push_back({from, to, CAPTURE, NO_PROMO, -1});
        }
        while (normal)
        {
            int to = pop_lsb(normal);
            list.push_back({from, to, 0, NO_PROMO, -1});
        }
    }
}

void generateRookMoves(const Position &pos, std::vector<Move> &list)
{
    Bitboard rooks = (pos.side_to_move == WHITE) ? pos.R : pos.r;
    Bitboard opp = opp_piece(pos, pos.side_to_move);
    while (rooks)
    {
        int from = pop_lsb(rooks);
        Bitboard moves = computeRookMoves(pos.side_to_move, pos, from);
        Bitboard captures = moves & opp;
        Bitboard normal = moves & ~pos.total_pieces;
        while (captures)
        {
            int to = pop_lsb(captures);
            list.push_back({from, to, CAPTURE, NO_PROMO, -1});
        }
        while (normal)
        {
            int to = pop_lsb(normal);
            list.push_back({from, to, 0, NO_PROMO, -1});
        }
    }
}

void generateQueenMoves(const Position &pos, std::vector<Move> &list)
{
    Bitboard queens = (pos.side_to_move == WHITE) ? pos.Q : pos.q;
    Bitboard opp = opp_piece(pos, pos.side_to_move);
    while (queens)
    {
        int from = pop_lsb(queens);
        Bitboard moves = computeQueenMoves(pos.side_to_move, pos, from);
        Bitboard captures = moves & opp;
        Bitboard normal = moves & ~pos.total_pieces;

        while (captures)
        {
            int to = pop_lsb(captures);
            list.push_back({from, to, CAPTURE, NO_PROMO, -1});
        }
        while (normal)
        {
            int to = pop_lsb(normal);
            list.push_back({from, to, 0, NO_PROMO, -1});
        }
    }
}

void generateAllMoves(const Position &pos, std::vector<Move> &list)
{
    generatePawnMoves(pos, list);
    generateKnightMoves(pos, list);
    generateBishopMoves(pos, list);
    generateRookMoves(pos, list);
    generateQueenMoves(pos, list);
    generateKingMoves(pos, list);
}

void generateLegalAllMoves(Position &pos, std::vector<Move> &final)
{
    std::vector<Move> temp;
    generateAllMoves(pos, temp);
    Color c = pos.side_to_move;

    for (int i = 0; i < (int)temp.size(); i++)
    {
        makeMove(pos, temp[i]);
        if (!isKinginCheck(c, pos))
        {
            final.push_back(temp[i]);
        }
        UndoMove(pos);
    }
}
