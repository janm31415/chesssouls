#pragma once

#include "libchesssouls_api.h"
#include <stdint.h>

class position;

LIB_CHESSSOULS_API uint64_t perft(position& pos, int depth);