#include "hash.h"
//#include "rkiss.h"
#include "rand.h"
#include <cstdlib>


uint64_t hash_piece[nr_color][nr_piecetype][nr_squares];
uint64_t hash_side;
uint64_t hash_ep[nr_squares];
//uint64_t hash_castle[16];

namespace
  {
  //RKISS rk;
  xorshift64star rk;
  }

uint64_t hash_rand()
  {
  int i;
  uint64_t r = 0;
  for (i = 0; i < 64; ++i)
    r ^= uint64_t(rk()) << i;
    //r ^= uint64_t(rk.rand<uint64_t>()) << i;
  return r;
  }

void init_hash()
  {
  int i, j, k;
  for (i = 0; i < nr_color; ++i)
    for (j = 0; j < nr_piecetype; ++j)
      for (k = 0; k < nr_squares; ++k)
        hash_piece[i][j][k] = hash_rand();
  hash_side = hash_rand();
  for (i = 0; i < nr_squares; ++i)
    hash_ep[i] = hash_rand();
  //for (i = 0; i < 16; ++i)
  //  hash_castle[i] = hash_rand();
  }