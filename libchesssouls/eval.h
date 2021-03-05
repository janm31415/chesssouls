#pragma once

#include "libchesssouls_api.h"
#include "types.h"
#include <stdint.h>
#include <limits>

class position;

extern int pcsq[2][7][nr_squares];

extern int piece_value[7];

LIB_CHESSSOULS_API void init_eval();

LIB_CHESSSOULS_API int eval(const position& pos);

typedef int score;

inline score make_score(int mid_game, int end_game) { return ((uint32_t)mid_game << 16) + end_game; }

inline int eg_value(score s) 
  {
  union { uint16_t u; int16_t s; } eg = { uint16_t(unsigned(s + 0x8000) >> 16) };
  return (int)(eg.s);
  }

inline int mg_value(score s) 
  {
  union { uint16_t u; int16_t s; } mg = { uint16_t(unsigned(s)) };
  return (int)(mg.s);
  }

enum e_value
  {
  pawn_value_mg = 100,
  pawn_value_eg = 130,
  knight_value_mg = 400,
  knight_value_eg = 420,
  bishop_value_mg = 400,
  bishop_value_eg = 420,
  rook_value_mg = 600,
  rook_value_eg = 600,
  queen_value_mg = 1200,
  queen_value_eg = 1200,
  midgame_limit = 7500,
  endgame_limit = 2000,
  value_mate = 10000,


  value_max = std::numeric_limits<int>::max(),
  value_min = std::numeric_limits<int>::min()
  };