#pragma once

#include <stdint.h>
#include "libchesssouls_api.h"

struct pawn_entry // 128 bits
  {
  uint32_t lock;
  uint64_t pawn_rank;
  int16_t white_pawn_eval;
  int16_t black_pawn_eval;
  };

struct pawn_table
  {
  pawn_entry* table;
  uint32_t mask;
  uint32_t size;
  };

extern pawn_table pawntable;

pawn_entry* get_pawn_entry(pawn_table& p, uint64_t h);
void init_pawn_table(pawn_table& p, uint32_t Mb = 8);
void destroy_pawn_table(pawn_table& p);
void clear_pawn_table(pawn_table& p);
void store_in_pawn_table(pawn_table& p, uint64_t hash, uint64_t pawn_rank, int16_t white_pawn_eval, int16_t black_pawn_eval);
bool find_in_pawn_table(pawn_table& p, uint64_t hash, uint64_t& pawn_rank, int16_t& white_pawn_eval, int16_t& black_pawn_eval);

LIB_CHESSSOULS_API void init_pawn_table(uint32_t Mb = 8);
LIB_CHESSSOULS_API void destroy_pawn_table();
LIB_CHESSSOULS_API void clear_pawn_table();