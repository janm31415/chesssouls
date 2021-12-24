#pragma once

#include <stdint.h>
#include "libchesssouls_api.h"
#include "types.h"

#define PV_NODE 0 // score is exact
#define ALPHA_NODE 1 // score is upper bound
#define BETA_NODE 2 // score is lower bound

#define OPPOSITE(node) ((node) ? (node) ^ 3 : (node)) 

#define KEY_INDEX(key) (uint32_t(key))
#define KEY_LOCK(key)  (uint32_t((key)>>32))

#define GET_FIRST_AGE(ages) ((ages) & 15)
#define GET_SECOND_AGE(ages) (((ages) & 240) >> 4)

#define GET_FIRST_NODETYPE(node) ((node) & 3)
#define GET_SECOND_NODETYPE(node) (((node) & 12) >> 2)


struct entry //128 bits
  {
  uint32_t lock; //32
  move move1; // 16
  move move2; // 16
  int8_t depth1; // 8
  int8_t depth2; // 8
  int16_t score1; // 16
  int16_t score2; // 16
  uint8_t ages; // 8
  uint8_t node_types; // 8
  };

struct transposition_table
  {
  entry* table;
  uint8_t age;
  uint32_t mask;
  uint32_t size;
  };

extern transposition_table transposition;

entry* get_entry(transposition_table& trans, uint64_t h);
void init_transposition_table(transposition_table& trans, uint32_t Mb = 64);
void destroy_transposition_table(transposition_table& trans);
void clear_transposition_table(transposition_table& trans);
void store_in_transposition_table(transposition_table& trans, uint64_t hash, const move& m, int depth, int score, int node_type);
bool find_in_transposition_table(transposition_table& trans, uint64_t hash, move& move1, int& depth1, int& score1, int& node_type1, move& move2, int& depth2, int& score2, int& node_type2);
void increase_transposition_age(transposition_table& trans);
//void dump_to_file(transposition_table& trans, const char* filename);

LIB_CHESSSOULS_API void init_transposition_table(uint32_t Mb = 64);
LIB_CHESSSOULS_API void destroy_transposition_table();
LIB_CHESSSOULS_API void clear_transposition_table();
LIB_CHESSSOULS_API void increase_transposition_age();