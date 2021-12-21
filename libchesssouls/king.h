#pragma once

#include <stdint.h>
#include "libchesssouls_api.h"

struct king_entry // 64 bits
  {
  uint32_t lock;  
  int16_t white_king_shield;
  int16_t black_king_shield;
  };

struct king_table
  {
  king_entry* table;
  uint32_t mask;
  uint32_t size;
  };

extern king_table kingtable;

king_entry* get_king_entry(king_table& k, uint64_t h);
void init_king_table(king_table& k, uint32_t Mb = 8);
void destroy_king_table(king_table& k);
void clear_king_table(king_table& k);
void store_in_king_table(king_table& p, uint64_t hash, int16_t white_king_shield, int16_t black_king_shield);
bool find_in_king_table(king_table& p, uint64_t hash, int16_t& white_king_shield, int16_t& black_king_shield);

LIB_CHESSSOULS_API void init_king_table(uint32_t Mb = 8);
LIB_CHESSSOULS_API void destroy_king_table();