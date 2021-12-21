#pragma once

#include "libchesssouls_api.h"
#include "types.h"
#include <stdint.h>
#include <limits>
#include <ostream>

class position;

extern int pcsq[2][7][nr_squares];

extern int piece_value[7];
extern int piece_value_see[7];

LIB_CHESSSOULS_API void init_eval();

LIB_CHESSSOULS_API int eval(const position& pos);
LIB_CHESSSOULS_API int eval_lazy(const position& pos);
LIB_CHESSSOULS_API int eval(const position& pos, int alpha, int beta);

typedef int score;

inline score make_score(int mid_game, int end_game) 
  { 
  return (int32_t)((uint32_t)end_game << 16) + mid_game; 
  }

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
  /*
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
  */
  
  pawn_value_mg = 60,
  pawn_value_eg = 100,
  knight_value_mg = 325,
  knight_value_eg = 335,
  bishop_value_mg = 335,
  bishop_value_eg = 350,
  rook_value_mg = 500,
  rook_value_eg = 590,
  queen_value_mg = 985,
  queen_value_eg = 1200,
  midgame_limit = 7500,
  endgame_limit = 2000,
  value_mate = 10000,  

  value_max = std::numeric_limits<int>::max(),
  value_min = std::numeric_limits<int>::min(),

  doubled_pawn_penalty = 10,
  isolated_pawn_penalty = 25,
  backwards_pawn_penalty = 8,
  passed_pawn_bonus = 30,
  rook_semi_open_file_bonus = 5,
  rook_open_file_bonus = 20,
  rook_on_seventh_bonus = 20,
  two_bishops_exist = 40,
  mobility_bonus = 10,
  trapped_bishop = 100,
  blocked_bishop = 50,
  blocked_rook = 50,
  knight_attack_value = 20,
  bishop_attack_value = 20,
  rook_attack_value = 40,
  queen_attack_value = 80,

  lazy_eval_margin = 250
  };

LIB_CHESSSOULS_API void print_eval(std::ostream& str, const position& pos);