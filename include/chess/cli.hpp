#pragma once
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <cctype>
#include "chess/types.hpp"
#include "chess/bitboard.hpp"
#include "chess/move.hpp"
#include "chess/position.hpp"
#include "chess/movegen.hpp"
#include "chess/make_undo.hpp"
#include "chess/status.hpp"

static inline void wait_for_enter();

inline std::string sq_to_str(int idx);
inline char promo_char(uint8_t promo);
inline std::string move_to_uci(const Move &m);
inline const char *side_name(Color c);
inline char piece_to_char(Piece p);
inline std::string board_row_string(const Position &pos, int rank);
inline void clear_screen();

void print_board(Bitboard board);
void print_board_with_legal(const Position &pos,
                            const std::vector<Move> &legal,
                            const std::vector<std::string> &history,
                            const std::string &last_move_uci);

Move convert_command(Position &pos, std::string command);
void gameloop();
