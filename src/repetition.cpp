#include "chess/repetition.hpp"
#include "chess/position.hpp"

std::vector<std::uint64_t> g_rep_stack;

void rep_init(const Position& pos) {
    g_rep_stack.clear();
    g_rep_stack.push_back(pos.zobrist);
}

void rep_push(const Position& pos) {
    g_rep_stack.push_back(pos.zobrist);
}

void rep_pop() {
    if (!g_rep_stack.empty()) g_rep_stack.pop_back();
}

int rep_count_current(const Position& pos, int max_plies_window) {
    if (g_rep_stack.empty()) return 0;
    const std::uint64_t key = pos.zobrist;

    int count = 0;
    int start = static_cast<int>(g_rep_stack.size()) - 1;       
    int stop  = std::max(0, start - max_plies_window);          

    for (int i = start; i >= stop; --i) {
        if (g_rep_stack[i] == key) ++count;
    }
    return count;
}

bool is_threefold(const Position& pos) {
    const int window = std::max(0, pos.halfmove);
    return rep_count_current(pos, window) >= 3;
}
