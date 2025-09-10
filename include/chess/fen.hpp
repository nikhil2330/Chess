#pragma once
#include <string>
#include "chess/types.hpp"
#include "chess/position.hpp"

bool loadFEN(Position &pos, std::string FENstr);
std::string saveFEN(const Position &pos);
