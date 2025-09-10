#include "chess/fen.hpp"
#include "chess/bitboard.hpp"
#include "chess/position.hpp"
#include "chess/attacks.hpp"
#include "chess/movegen.hpp"
#include "chess/cli.hpp"
#include "chess/zobrist.hpp"
#include "chess/repetition.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

bool loadFEN(Position &pos, std::string FENstr)
{
    pos.clear();
    std::istringstream ss(FENstr);
    std::string field;
    std::vector<std::string> fields;
    Bitboard board;
    while (ss >> field)
    {
        fields.push_back(field);
    }
    if (fields.size() < 4)
        return false;
    int file = 0;
    int rank = 7;
    for (int i = 0; i < (int)fields[0].length(); i++)
    {
        if (fields[0][i] == '/')
        {
            if (file != 8)
                return false;
            rank--;
            file = 0;
            continue;
        }
        if (fields[0][i] >= '1' && fields[0][i] <= '8')
        {
            file += (fields[0][i] - '0');
            if (file > 8)
                return false;
            continue;
        }

        Piece p = EMPTY;
        switch (fields[0][i])
        {
        case 'P':
            p = WP;
            break;
        case 'N':
            p = WN;
            break;
        case 'B':
            p = WB;
            break;
        case 'R':
            p = WR;
            break;
        case 'Q':
            p = WQ;
            break;
        case 'K':
            p = WK;
            break;
        case 'p':
            p = BP;
            break;
        case 'n':
            p = BN;
            break;
        case 'b':
            p = BB;
            break;
        case 'r':
            p = BR;
            break;
        case 'q':
            p = BQ;
            break;
        case 'k':
            p = BK;
            break;
        default:
            return false;
            break;
        }
        if (p != EMPTY)
        {
            int sq = rank * 8 + file;
            if (sq < 0 || sq > 63)
                return false;
            addPiece(pos, sq, p);
            file++;
        }
    }
    if (rank != 0 || file != 8)
        return false;

    if (fields[1][0] == 'w')
        pos.side_to_move = WHITE;
    else if (fields[1][0] == 'b')
        pos.side_to_move = BLACK;
    else
        return false;

    pos.castling = 0;
    for (int i = 0; i < (int)fields[2].length(); i++)
    {
        if (fields[2][i] == 'K')
            pos.castling |= White_King;
        if (fields[2][i] == 'Q')
            pos.castling |= White_Queen;
        if (fields[2][i] == 'k')
            pos.castling |= Black_King;
        if (fields[2][i] == 'q')
            pos.castling |= Black_Queen;
    }

    if (fields[3][0] != '-')
    {
        if (fields[3].size() != 2)
            return false;
        char f = fields[3][0], r = fields[3][1];
        if (f < 'a' || f > 'h' || r < '1' || r > '8')
            return false;
        pos.en_passant = get_index(f, r - '0');
    }

    auto is_all_digits = [](const std::string &s)
    {
        return !s.empty() &&
               std::all_of(s.begin(), s.end(), [](unsigned char ch)
                           { return std::isdigit(ch); });
    };

    if (fields.size() >= 5)
    {
        if (!is_all_digits(fields[4]))
            return false;
        pos.halfmove = std::stoi(fields[4]);
    }
    else
        pos.halfmove = 0;

    if (fields.size() >= 6)
    {
        if (!is_all_digits(fields[5]))
            return false;
        pos.fullmove = std::max(1, std::stoi(fields[5]));
    }
    else
        pos.fullmove = 1;

    pos.board_state();
    pos.zobrist = Zobrist::compute(pos);
    rep_init(pos);                       // <-- ADD THIS
    return true;    

}

std::string saveFEN(const Position &pos)
{
    std::string FENstr;
    int rank = 7;
    int file = 0;
    int x = 0;
    for (int rank = 7; rank >= 0; rank--)
    {
        int x = 0;
        for (int file = 0; file < 8; file++)
        {
            int sq = rank * 8 + file;
            char c = piece_to_char(getPiece(pos, sq));
            if (c == '.')
            {
                x++;
            }
            else
            {
                if (x)
                {
                    FENstr += '0' + x;
                    x = 0;
                }
                FENstr += c;
            }
        }
        if (x)
            FENstr += '0' + x;
        if (rank > 0)
            FENstr += '/';
    }

    FENstr += ' ';

    pos.side_to_move == WHITE ? FENstr += 'w' : FENstr += 'b';

    FENstr += ' ';

    if (pos.castling == 0)
        FENstr += '-';
    if (pos.castling & White_King)
        FENstr += 'K';
    if (pos.castling & White_Queen)
        FENstr += 'Q';
    if (pos.castling & Black_King)
        FENstr += 'k';
    if (pos.castling & Black_Queen)
        FENstr += 'q';

    FENstr += ' ';

    if (pos.en_passant != -1)
    {
        FENstr += sq_to_str(pos.en_passant);
    }
    else
    {
        FENstr += '-';
    }
    FENstr += ' ';
    FENstr += std::to_string(pos.halfmove);
    FENstr += ' ';
    FENstr += std::to_string(pos.fullmove);
    return FENstr;
}
