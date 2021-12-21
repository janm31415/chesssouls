#pragma once

#include <stdint.h>
#include "libchesssouls_api.h"

struct eval_entry // 64 bits
  {
  uint32_t lock;
  int32_t eval_white;
  };

struct eval_table
  {
  eval_entry* table;
  uint32_t mask;
  uint32_t size;
  };

extern eval_table evaltable;

eval_entry* get_eval_entry(eval_table& p, uint64_t h);
void init_eval_table(eval_table& p, uint32_t Mb = 64);
void destroy_eval_table(eval_table& p);
void clear_eval_table(eval_table& p);
void store_in_eval_table(eval_table& p, uint64_t hash, int32_t eval_white);
bool find_in_eval_table(eval_table& p, uint64_t hash, int32_t& eval_white);

LIB_CHESSSOULS_API void init_eval_table(uint32_t Mb = 64);
LIB_CHESSSOULS_API void destroy_eval_table();