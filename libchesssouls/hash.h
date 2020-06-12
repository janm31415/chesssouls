#pragma once

#include "types.h"
#include "libchesssouls_api.h"

extern uint64_t hash_piece[nr_color][nr_piecetype][nr_squares];
extern uint64_t hash_side;
extern uint64_t hash_ep[nr_squares];
extern uint64_t hash_castle[16];

uint64_t hash_rand();
LIB_CHESSSOULS_API void init_hash();