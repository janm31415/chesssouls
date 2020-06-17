#pragma once

#include "libchesssouls_api.h"
#include "types.h"
#include <string>

class position;

LIB_CHESSSOULS_API void read_book(const std::string& book_filename);
LIB_CHESSSOULS_API void clear_book();
LIB_CHESSSOULS_API move book_move(const position& pos);