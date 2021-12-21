#include "king.h"
#include "hash.h"
#include "trans.h"

king_table kingtable;

king_entry* get_king_entry(king_table& k, uint64_t h)
  {
  uint32_t index = KEY_INDEX(h) & k.mask;
  return &k.table[index];
  }

void init_king_table(king_table& k, uint32_t Mb)
  {
  uint32_t target = (Mb * 1024 * 1024) / sizeof(king_entry);
  uint32_t size = 1;
  while (size <= target)
    size *= 2;
  size /= 2; // size is a power of 2  
  k.size = size;
  k.mask = size - 1;
  k.table = new king_entry[k.size];
  clear_king_table(k);
  }

void destroy_king_table(king_table& k)
  {
  delete[] k.table;
  }

void clear_king_table(king_table& k)
  {
  king_entry empty;
  empty.lock = 0;
  empty.white_king_shield = 0;
  empty.white_king_shield = 0;

  king_entry* e = k.table;

  for (uint32_t index = 0; index < k.size; ++index)
    {
    *e++ = empty;
    }
  }

void store_in_king_table(king_table& k, uint64_t hash, int16_t white_king_shield, int16_t black_king_shield)
  {
  king_entry* e = get_king_entry(k, hash);
  e->lock = KEY_LOCK(hash);
  e->white_king_shield = white_king_shield;
  e->black_king_shield = black_king_shield;
  }

bool find_in_king_table(king_table& k, uint64_t hash, int16_t& white_king_shield, int16_t& black_king_shield)
  {
  king_entry* e = get_king_entry(k, hash);
  if (e->lock == KEY_LOCK(hash))
    {
    white_king_shield = e->white_king_shield;
    black_king_shield = e->black_king_shield;
    return true;
    }
  return false;
  }

void init_king_table(uint32_t Mb)
  {
  init_king_table(kingtable, Mb);
  }

void destroy_king_table()
  {
  destroy_king_table(kingtable);
  }

void clear_king_table()
  {
  clear_king_table(kingtable);
  }