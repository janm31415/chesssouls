#pragma once

#include "types.h"
#include "libchesssouls_api.h"
#include <string>

class position;

char to_char(e_file f, bool tolower = true);
char to_char(e_rank r);

const std::string to_string(e_square s);

LIB_CHESSSOULS_API std::string move_to_uci(move m);
LIB_CHESSSOULS_API move parse_move(const position& pos, const std::string& str);
