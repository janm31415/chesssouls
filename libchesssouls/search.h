#pragma once
#include "libchesssouls_api.h"
#include "types.h"

LIB_CHESSSOULS_API extern pv_line pv[max_ply]; // principal variation
LIB_CHESSSOULS_API extern int max_depth;
LIB_CHESSSOULS_API extern int time_limit;
LIB_CHESSSOULS_API extern uint64_t node_limit;
LIB_CHESSSOULS_API extern bool stop_search;
LIB_CHESSSOULS_API extern bool use_book;
extern int history[nr_squares][nr_squares];
extern bool follow_pv;
extern int ply;


class position;

LIB_CHESSSOULS_API void think(position& pos, int output);