#pragma once
#include <cstdint>
#include <vector>

struct Position;                 // forward-declare

extern std::vector<std::uint64_t> g_rep_stack;

void rep_init(const Position& pos);

void rep_push(const Position& pos);

void rep_pop();

int  rep_count_current(const Position& pos, int max_plies_window);

bool is_threefold(const Position& pos);
