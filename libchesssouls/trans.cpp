#include "trans.h"
#include <cassert>

transposition_table transposition;

const int max_age = 16;
const int cluster_size = 4;
const int invalid_depth = -64;

inline void store_ages(uint8_t& ages, uint8_t age1, uint8_t age2)
  {
  ages = age1 + (age2 << 4);
  assert(GET_FIRST_AGE(ages) == age1);
  assert(GET_SECOND_AGE(ages) == age2);
  }

inline void store_nodetypes(uint8_t& nodetypes, uint8_t node1, uint8_t node2)
  {
  nodetypes = node1 + (node2 << 2);
  assert(GET_FIRST_NODETYPE(nodetypes) == node1);
  assert(GET_SECOND_NODETYPE(nodetypes) == node2);
  }

entry* get_entry(transposition_table& trans, uint64_t h)
  {
  uint32_t index = KEY_INDEX(h) & trans.mask;
  return &trans.table[index];
  }

void init_transposition_table(transposition_table& trans, uint32_t Mb)
  {
  uint32_t target = (Mb * 1024 * 1024) / sizeof(entry);
  uint32_t size = 1;
  while (size <= target)
    size *= 2;
  size /= 2; // size is a power of 2  
  trans.size = size + (cluster_size - 1);
  trans.mask = size - 1;
  trans.table = new entry[trans.size];
  clear_transposition_table(trans);
  }

void destroy_transposition_table(transposition_table& trans)
  {
  delete[] trans.table;
  }

void increase_transposition_age(transposition_table& trans)
  {
  trans.age = (trans.age + 1) % max_age;
  }

void clear_transposition_table(transposition_table& trans)
  {
  entry empty;
  empty.lock = 0;
  empty.ages = 0;
  empty.depth1 = invalid_depth;
  empty.depth2 = invalid_depth;
  empty.move1 = 0;
  empty.move2 = 0;
  empty.node_types = 0;
  empty.score1 = -10000;
  empty.score2 = -10000;

  entry* e = trans.table;

  for (uint32_t index = 0; index < trans.size; ++index)
    {
    *e++ = empty;
    }
  }

void store_in_transposition_table(transposition_table& trans, uint64_t hash, const move& m, int depth, int score, int node_type)
  {
  entry* e;
  entry* best_e = nullptr;

  int s;
  int best_s = -10000;

  e = get_entry(trans, hash);
  for (int i = 0; i < cluster_size; ++i, ++e)
    {
    if (e->lock == KEY_LOCK(hash))
      {
      uint8_t node_type2;
      uint8_t age2;
      e->depth1 = depth;
      e->move1 = m;
      e->score1 = score;
      if (e->depth2 <= depth)
        {
        e->depth2 = depth;
        e->move2 = e->move1;
        e->score2 = score;
        age2 = trans.age;
        node_type2 = node_type;
        }
      else
        {
        node_type2 = GET_SECOND_NODETYPE(e->node_types);
        age2 = GET_SECOND_AGE(e->ages);
        }

      store_ages(e->ages, trans.age, age2);
      store_nodetypes(e->node_types, node_type, node_type2);
      return;
      }
    uint8_t age1 = GET_FIRST_AGE(e->ages);
    if (age1 <= trans.age)
      s = int(trans.age - age1) * 256 - e->depth1;
    else
      s = int(trans.age + max_age - age1) * 256 - e->depth1;
    if (s > best_s)
      {
      best_s = s;
      best_e = e;
      }
    }
  e = best_e;
  e->lock = KEY_LOCK(hash);
  e->depth1 = depth;
  e->depth2 = depth;
  e->move1 = m;
  e->move2 = e->move1;
  e->score1 = score;
  e->score2 = score;
  store_ages(e->ages, trans.age, trans.age);
  store_nodetypes(e->node_types, node_type, node_type);
  }

bool find_in_transposition_table(transposition_table& trans, uint64_t hash, move& move1, int& depth1, int& score1, int& node_type1, move& move2, int& depth2, int& score2, int& node_type2)
  {
  entry* e = get_entry(trans, hash);
  for (int i = 0; i < cluster_size; ++i, ++e)
    {
    if (e->lock == KEY_LOCK(hash))
      {
      store_ages(e->ages, trans.age, trans.age);
      move1 = e->move1;
      depth1 = e->depth1;
      score1 = e->score1;
      node_type1 = GET_FIRST_NODETYPE(e->node_types);
      move2 = e->move2;
      depth2 = e->depth2;
      score2 = e->score2;
      node_type2 = GET_SECOND_NODETYPE(e->node_types);
      return true;
      }
    }
  return false;
  }
/*
void dump_to_file(transposition_table& trans, const char* filename)
  {
  FILE* p_file;
  p_file = fopen(filename, "wb");
  fwrite(trans.table, sizeof(entry), trans.size, p_file);
  fclose(p_file);
  }
*/

void init_transposition_table(uint32_t Mb)
  {
  init_transposition_table(transposition, Mb);
  }

void destroy_transposition_table()
  {
  destroy_transposition_table(transposition);
  }

void clear_transposition_table()
  {
  clear_transposition_table(transposition);
  }

void increase_transposition_age()
  {
  increase_transposition_age(transposition);
  }