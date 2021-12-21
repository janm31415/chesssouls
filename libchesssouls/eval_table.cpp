#include "eval_table.h"
#include "hash.h"
#include "trans.h"

eval_table evaltable;

eval_entry* get_eval_entry(eval_table& p, uint64_t h)
  {
  uint32_t index = KEY_INDEX(h) & p.mask;
  return &p.table[index];
  }

void init_eval_table(eval_table& p, uint32_t Mb)
  {
  uint32_t target = (Mb * 1024 * 1024) / sizeof(eval_entry);
  uint32_t size = 1;
  while (size <= target)
    size *= 2;
  size /= 2; // size is a power of 2  
  p.size = size;
  p.mask = size - 1;
  p.table = new eval_entry[p.size];
  clear_eval_table(p);
  }

void destroy_eval_table(eval_table& p)
  {
  delete[] p.table;
  }

void clear_eval_table(eval_table& p)
  {
  eval_entry empty;
  empty.lock = 0;
  empty.eval_white = 0;  

  eval_entry* e = p.table;

  for (uint32_t index = 0; index < p.size; ++index)
    {
    *e++ = empty;
    }
  }

void store_in_eval_table(eval_table& p, uint64_t hash, int32_t eval_white)
  {
  eval_entry* e = get_eval_entry(p, hash);
  e->lock = KEY_LOCK(hash);
  e->eval_white = eval_white;
  }

bool find_in_eval_table(eval_table& p, uint64_t hash, int32_t& eval_white)
  {
  eval_entry* e = get_eval_entry(p, hash);
  if (e->lock == KEY_LOCK(hash))
    {
    eval_white = e->eval_white;
    return true;
    }
  return false;
  }

void init_eval_table(uint32_t Mb)
  {
  init_eval_table(evaltable, Mb);
  }

void destroy_eval_table()
  {
  destroy_eval_table(evaltable);
  }

void clear_eval_table()
  {
  clear_eval_table(evaltable);
  }