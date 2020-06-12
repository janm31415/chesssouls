#include "bitboard.h"

bitboard square[nr_squares];
bitboard file[nr_files];
bitboard rank[nr_ranks];
bitboard adjacent_files[nr_files];

void init_bitboards()
  {
  for (auto s = sq_a1; s < sq_end; ++s)
    {
    square[s] = (uint64_t)1 << s;
    }

  for (auto f = file_a; f < file_end; ++f)
    {
    file[f] = f > file_a ? file[f - 1] << 1 : fileA;
    }

  for (auto r = rank_1; r < rank_end; ++r)
    {
    rank[r] = r > rank1 ? rank[r - 1] << 8 : rank1;
    }

  for (auto f = file_a; f < file_end; ++f)
    {
    adjacent_files[f] = (f > file_a ? file[f - 1] : 0) | (f < file_h ? file[f + 1] : 0);
    }

  }