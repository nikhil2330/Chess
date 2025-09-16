#include "chess/cli.hpp"

static inline void wait_for_enter()
{
    std::cout << "\n\n(press Enter to return)";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

inline std::string sq_to_str(int idx)
{
    if (idx < 0 || idx > 63)
        return "--";
    char f = 'a' + (idx % 8);
    char r = '1' + (idx / 8);
    return std::string{f, r};
}

inline char promo_char(uint8_t promo)
{
    switch (promo)
    {
    case PROMO_Q:
        return 'q';
    case PROMO_R:
        return 'r';
    case PROMO_B:
        return 'b';
    case PROMO_N:
        return 'n';
    default:
        return 0;
    }
}

inline std::string move_to_uci(const Move &m)
{
    std::string s;
    if (m.from >= 0 && m.to >= 0)
    {
        s += sq_to_str(m.from);
        s += sq_to_str(m.to);
        if (m.flags & PROMOTION)
            s.push_back(promo_char(m.promo));
    }
    return s;
}

inline const char *side_name(Color c) { return c == WHITE ? "White" : "Black"; }

inline char piece_to_char(Piece p)
{
    switch (p)
    {
    case WP:
        return 'P';
    case WN:
        return 'N';
    case WB:
        return 'B';
    case WR:
        return 'R';
    case WQ:
        return 'Q';
    case WK:
        return 'K';
    case BP:
        return 'p';
    case BN:
        return 'n';
    case BB:
        return 'b';
    case BR:
        return 'r';
    case BQ:
        return 'q';
    case BK:
        return 'k';
    default:
        return '.';
    }
}

inline std::string board_row_string(const Position &pos, int rank)
{
    std::ostringstream oss;
    oss << (rank + 1) << "   ";
    for (int file = 0; file < 8; ++file)
    {
        int idx = rank * 8 + file;
        oss << piece_to_char(getPiece(pos, idx)) << ' ';
    }
    return oss.str();
}

inline void clear_screen()
{
#ifdef _WIN32
    std::system("cls");
#else
    std::cout << "\x1b[2J\x1b[H";
#endif
}

void print_board_with_legal(const Position &pos,
                                   const std::vector<Move> &legal,
                                   const std::vector<std::string> &history,
                                   const std::string &last_move_uci)
{
    clear_screen();

    // Header
    std::cout << "Mode: 2-Player   "
              << "Turn: " << side_name(pos.side_to_move)
              << "    Fullmove: " << pos.fullmove
              << "    Halfmove: " << pos.halfmove
              << "    Last move: " << (last_move_uci.empty() ? "-" : last_move_uci)
              << "\n";

    std::cout << "Castling: "
              << ((pos.castling & White_King) ? 'K' : '-')
              << ((pos.castling & White_Queen) ? 'Q' : '-')
              << ((pos.castling & Black_King) ? 'k' : '-')
              << ((pos.castling & Black_Queen) ? 'q' : '-');

    std::cout << "    EP: ";
    if (pos.en_passant == -1)
        std::cout << "-";
    else
        std::cout << sq_to_str(pos.en_passant);
    std::cout << "    Legal moves: " << legal.size() << "\n\n";

    std::vector<std::string> boardLines;
    for (int r = 7; r >= 0; --r)
        boardLines.push_back(board_row_string(pos, r));

    std::vector<std::string> legalLines;
    size_t show = std::min<size_t>(8, legal.size());
    for (size_t i = 0; i < show; ++i)
    {
        std::ostringstream os;
        os << std::setw(2) << (i + 1) << ") " << move_to_uci(legal[i]);
        legalLines.push_back(os.str());
    }
    while (legalLines.size() < 8)
        legalLines.push_back("");

    for (int i = 0; i < 8; ++i)
    {
        std::cout << std::left << std::setw(18) << legalLines[i] << " | " << boardLines[i] << "\n";
    }
    std::cout << "\n";
    std::cout << std::left << std::setw(18) << "" << " | "
              << "    a b c d e f g h\n";

    std::cout << "\nMoves played (" << history.size() << "): ";
    if (history.empty())
        std::cout << "-";
    else
    {
        for (size_t i = 0; i < history.size(); ++i)
        {
            if (i)
                std::cout << ' ';
            std::cout << history[i];
        }
    }
    std::cout << "\n";

    std::cout << "\nEnter move in (e2e4, g7g8q), or commands: "
              << "'undo', 'list' (show all legal), 'help', 'quit'\n";
}

Move convert_command(Position &pos, std::string command)
{
    Move m;
    m.from = -1;
    m.to = -1;
    m.flags = 0;
    m.promo = NO_PROMO;
    m.captured = -1;

    if (command.size() != 4 && command.size() != 5)
        return m;

    char f1 = command[0], r1 = command[1], f2 = command[2], r2 = command[3];
    if (f1 < 'a' || f1 > 'h' || f2 < 'a' || f2 > 'h' ||
        r1 < '1' || r1 > '8' || r2 < '1' || r2 > '8')
    {
        return m;
    }

    m.from = get_index(f1, r1 - '0');
    m.to = get_index(f2, r2 - '0');

    Bitboard fromBB = convert_to_bit(m.from);
    if ((ally_piece(pos, pos.side_to_move) & fromBB) == 0)
    {
        m.from = m.to = -1;
        return m;
    }

    Piece p = getPiece(pos, m.from);
    Bitboard opp = opp_piece(pos, pos.side_to_move);
    bool destHasOpp = (opp & convert_to_bit(m.to)) != 0;

    if (command.size() == 5)
    {
        char pc = command[4];
        if (pc == 'n' || pc == 'N')
        {
            m.flags |= PROMOTION;
            m.promo = PROMO_N;
        }
        else if (pc == 'b' || pc == 'B')
        {
            m.flags |= PROMOTION;
            m.promo = PROMO_B;
        }
        else if (pc == 'r' || pc == 'R')
        {
            m.flags |= PROMOTION;
            m.promo = PROMO_R;
        }
        else if (pc == 'q' || pc == 'Q')
        {
            m.flags |= PROMOTION;
            m.promo = PROMO_Q;
        }
        else
        {
            m.from = m.to = -1;
            return m;
        }
    }

    if (p == WP || p == BP)
    {
        bool fileChanged = (m.from % 8) != (m.to % 8);
        bool destEmpty = (getPiece(pos, m.to) == EMPTY);

        if (pos.en_passant != -1 && m.to == pos.en_passant && fileChanged && destEmpty)
        {
            m.flags |= EN_PASSANT;
        }
        else if (destHasOpp)
        {
            m.flags |= CAPTURE;
        }

        if (p == WP && (m.from / 8 == 1) && (m.to - m.from == 16))
            m.flags |= DOUBLE_PUSH;
        if (p == BP && (m.from / 8 == 6) && (m.from - m.to == 16))
            m.flags |= DOUBLE_PUSH;

        bool reachesLast = (p == WP && m.to / 8 == 7) || (p == BP && m.to / 8 == 0);
        if (reachesLast && (m.flags & PROMOTION) == 0)
        {
            m.flags |= PROMOTION;
            m.promo = PROMO_Q;
        }
    }
    else
    {
        if (destHasOpp)
            m.flags |= CAPTURE;
        if ((m.flags & PROMOTION) != 0)
        {
            m.from = m.to = -1;
            m.flags = 0;
            m.promo = NO_PROMO;
            return m;
        }
    }

    return m;
}

void gameloop()
{
    std::cout << "Welcome to Chess 1.0!\n";
    while (true)
    {
        std::cout << "\nMenu:\n"
                  << "  A) Player vs Computer (coming soon)\n"
                  << "  B) 2 Player (local)\n"
                  << "  C) Analysis (coming soon)\n"
                  << "  D) Quit\n"
                  << "Select: ";
        char option;
        std::cin >> option;
        option = std::toupper(option);

        if (option == 'D')
        {
            std::cout << "Quitting\n";
            break;
        }
        if (option == 'A' || option == 'C')
        {
            std::cout << "Not implemented yet. Pick B for 2-Player.\n";
            continue;
        }
        if (option == 'B')
        {
            Position game;
            game.start_position();
            GameStatus status = assessStatus(game);

            std::vector<Move> moveHistory;
            std::vector<std::string> moveHistoryStr;
            std::string lastMoveStr;

            while (status.phase == Phase::Playing)
            {
                std::vector<Move> legal;
                generateLegalAllMoves(game, legal);

                print_board_with_legal(game, legal, moveHistoryStr, lastMoveStr);

                if (status.in_check)
                {
                    std::cout << side_name(game.side_to_move) << " is in CHECK.\n";
                }

                std::cout << side_name(game.side_to_move) << " to move > ";
                std::string command;
                std::cin >> command;

                if (command == "quit" || command == "q")
                {
                    std::cout << "Leaving game.\n";
                    break;
                }
                if (command == "help")
                {
                    std::cout << "moves like e2e4, g7g8q. Commands: undo, list, quit.\n";
                    wait_for_enter();
                    continue;
                }
                if (command == "list")
                {
                    std::cout << "All legal moves (" << legal.size() << "):\n";
                    for (size_t i = 0; i < legal.size(); ++i)
                    {
                        std::cout << std::setw(6) << move_to_uci(legal[i]);
                        if ((i + 1) % 12 == 0)
                            std::cout << '\n';
                    }
                    wait_for_enter();
                    continue;
                }
                if (command == "undo")
                {
                    if (moveHistory.empty())
                    {
                        std::cout << "Nothing to undo.\n";
                        continue;
                    }
                    UndoMove(game);
                    moveHistory.pop_back();
                    moveHistoryStr.pop_back();
                    lastMoveStr = moveHistoryStr.empty() ? "" : moveHistoryStr.back();
                    status = assessStatus(game);
                    continue;
                }

                Move typed = convert_command(game, command);
                if (typed.from < 0 || typed.to < 0)
                {
                    std::cout << "Bad move format. Use e2e4 or g7g8q.\n";
                    wait_for_enter();
                    continue;
                }

                auto it = std::find(legal.begin(), legal.end(), typed);
                if (it == legal.end())
                {
                    std::cout << "Illegal move.\n";
                    wait_for_enter();
                    continue;
                }

                makeMove(game, *it);
                std::string uci = move_to_uci(*it);
                moveHistory.push_back(*it);
                moveHistoryStr.push_back(uci);
                lastMoveStr = uci;

                status = assessStatus(game);
                if (status.phase == Phase::GameOver)
                {
                    print_board_with_legal(game, {}, moveHistoryStr, lastMoveStr);
                    if (status.outcome == Outcome::Whitewins)
                    {
                        std::cout << "Checkmate! White wins.\n";
                    }
                    else if (status.outcome == Outcome::Blackwins)
                    {
                        std::cout << "Checkmate! Black wins.\n";
                    }
                    else
                    {
                        std::cout << "Draw: ";
                        switch (status.draw_reason)
                        {
                        case DrawReason::Stalemate:
                            std::cout << "stalemate";
                            break;
                        case DrawReason::InsufficientMaterial:
                            std::cout << "insufficient material";
                            break;
                        case DrawReason::FiftyMove:
                            std::cout << "50-move rule";
                            break;
                        case DrawReason::Threefold:
                            std::cout << "threefold repetition";
                            break;
                        case DrawReason::Agreement:
                            std::cout << "by agreement";
                            break;
                        default:
                            std::cout << "unknown reason";
                            break;
                        }
                        std::cout << ".\n";
                    }
                    std::cout << "Press any key to return to menu...\n";
                    std::string a;
                    std::cin >> a;
                    break;
                }
            }
        }
    }
}
