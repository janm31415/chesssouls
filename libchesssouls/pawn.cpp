#include "pawn.h"
#include "hash.h"
#include "trans.h"

pawn_table pawntable;

pawn_entry* get_pawn_entry(pawn_table& p, uint64_t h)
  {
  uint32_t index = KEY_INDEX(h) & p.mask;
  return &p.table[index];
  }

void init_pawn_table(pawn_table& p, uint32_t Mb)
  {
  uint32_t target = (Mb * 1024 * 1024) / sizeof(pawn_entry);
  uint32_t size = 1;
  while (size <= target)
    size *= 2;
  size /= 2; // size is a power of 2  
  p.size = size;
  p.mask = size - 1;
  p.table = new pawn_entry[p.size];
  clear_pawn_table(p);
  }

void destroy_pawn_table(pawn_table& p)
  {
  delete[] p.table;
  }

void clear_pawn_table(pawn_table& p)
  {
  pawn_entry empty;
  empty.lock = 0;
  empty.pawn_rank = 0;
  empty.white_pawn_eval = 0;
  empty.black_pawn_eval = 0;

  pawn_entry* e = p.table;

  for (uint32_t index = 0; index < p.size; ++index)
    {
    *e++ = empty;
    }
  }

void store_in_pawn_table(pawn_table& p, uint64_t hash, uint64_t pawn_rank, int16_t white_pawn_eval, int16_t black_pawn_eval)
  {
  pawn_entry* e = get_pawn_entry(p, hash);
  e->lock = KEY_LOCK(hash);
  e->pawn_rank = pawn_rank;
  e->white_pawn_eval = white_pawn_eval;
  e->black_pawn_eval = black_pawn_eval;
  }

bool find_in_pawn_table(pawn_table& p, uint64_t hash, uint64_t& pawn_rank, int16_t& white_pawn_eval, int16_t& black_pawn_eval)
  {
  pawn_entry* e = get_pawn_entry(p, hash);
  if (e->lock == KEY_LOCK(hash))
    {
    pawn_rank = e->pawn_rank;
    white_pawn_eval = e->white_pawn_eval;
    black_pawn_eval = e->black_pawn_eval;
    return true;
    }
  return false;
  }

void init_pawn_table(uint32_t Mb)
  {
  init_pawn_table(pawntable, Mb);
  }

void destroy_pawn_table()
  {
  destroy_pawn_table(pawntable);
  }

void clear_pawn_table()
  {
  clear_pawn_table(pawntable);
  }