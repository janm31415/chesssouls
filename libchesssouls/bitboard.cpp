#include "bitboard.h"
#include "rkiss.h"
#include <algorithm>

#include <intrin.h>

bitboard square[nr_squares];
bitboard file[nr_files];
bitboard rank[nr_ranks];
bitboard adjacent_files[nr_files];
bitboard step_attacks[nr_pieces][nr_squares];
int square_distance_table[nr_squares][nr_squares];
bitboard ray_attack[8][nr_squares];
bitboard line[nr_squares][nr_squares];
bitboard between[nr_squares][nr_squares];
bitboard castling_path[9];
bitboard pseudo_attack[nr_piecetype][nr_squares];

namespace
  {
  //Bit scan:
  //https://www.chessprogramming.org/BitScan

  const uint64_t DeBruijn_64 = 0x3F79D71B4CB0A89ULL;
  int bit_scan_reverse_most_significant_1bit_table[256];
  e_square bit_scan_forward_table[nr_squares];
  inline unsigned int bit_scan_forward_index(bitboard b)
    {
    b ^= (b - 1);
    return (b * DeBruijn_64) >> 58;
    }



  bitboard sliding_attack(e_square delta, e_square sq, bitboard occupied)
    {
    bitboard attack = 0;

    for (e_square s = sq + delta; is_ok(s) && square_distance(s, s - delta) == 1; s += delta)
      {
      attack |= s;

      if (occupied & s)
        break;
      }

    return attack;
    }


  }

void init_bitboards()
  {
  for (auto s = sq_a1; s < sq_end; ++s)
    {
    square[s] = ((uint64_t)1) << s;
    bit_scan_forward_table[bit_scan_forward_index(square[s])] = s;
    }

  for (bitboard b = 1; b < 256; ++b)
    bit_scan_reverse_most_significant_1bit_table[b] = more_than_one(b) ? bit_scan_reverse_most_significant_1bit_table[b - 1] : least_significant_bit(b);

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

  for (e_square s1 = sq_a1; s1 <= sq_h8; ++s1)
    {
    for (e_square s2 = sq_a1; s2 <= sq_h8; ++s2)
      {
      square_distance_table[s1][s2] = std::max(file_distance(s1, s2), rank_distance(s1, s2));
      }
    }

  int steps[][9] = { { 0, 0, 0, 0, 0, 0, 0, 0, 0},
                     { 7, 9, 0, 0, 0, 0, 0, 0, 0 },
                     { 17, 15, 10, 6, -6, -10, -15, -17, 0 },
                     {0, 0, 0, 0, 0, 0, 0, 0, 0},
                     {0, 0, 0, 0, 0, 0, 0, 0, 0},
                     {0, 0, 0, 0, 0, 0, 0, 0, 0},
                     { 9, 7, -7, -9, 8, 1, -1, -8, 0} };

  for (e_color c = white; c <= black; ++c)
    {
    for (e_piecetype pt = pawn; pt <= king; ++pt)
      {
      for (e_square s = sq_a1; s <= sq_h8; ++s)
        {
        for (int i = 0; steps[pt][i]; ++i)
          {
          e_square to = s + e_square(c == white ? steps[pt][i] : -steps[pt][i]);
          if (is_ok(to) && square_distance(s, to) < 3)
            step_attacks[make_piece(c, pt)][s] |= to;
          }
        }
      }
    }

  for (e_square s = sq_a1; s <= sq_h8; ++s)
    {    
    ray_attack[north][s] = sliding_attack(sq_delta_up, s, 0);
    ray_attack[north_east][s] = sliding_attack(sq_delta_up_right, s, 0);
    ray_attack[east][s] = sliding_attack(sq_delta_right, s, 0);
    ray_attack[south_east][s] = sliding_attack(sq_delta_down_right, s, 0);
    ray_attack[south][s] = sliding_attack(sq_delta_down, s, 0);
    ray_attack[south_west][s] = sliding_attack(sq_delta_down_left, s, 0);
    ray_attack[west][s] = sliding_attack(sq_delta_left, s, 0);
    ray_attack[north_west][s] = sliding_attack(sq_delta_up_left, s, 0);
    }

  castling_path[1] = square[sq_f1] | square[sq_g1];
  castling_path[2] = square[sq_b1] | square[sq_c1] | square[sq_d1];
  castling_path[4] = square[sq_f8] | square[sq_g8];
  castling_path[8] = square[sq_b8] | square[sq_c8] | square[sq_d8];

  for (e_square s1 = sq_a1; s1 <= sq_h8; ++s1)
    {
    pseudo_attack[bishop][s1] = attacks_from_bishop(s1, 0);
    pseudo_attack[queen][s1] = pseudo_attack[bishop][s1];
    pseudo_attack[rook][s1] = attacks_from_rook(s1, 0);
    pseudo_attack[queen][s1] |= pseudo_attack[rook][s1];
    for (e_square s2 = sq_a1; s2 <= sq_h8; ++s2)
      {      
      if (pseudo_attack[bishop][s1] & s2)
        {
        line[s1][s2] = (attacks_from_bishop(s1, 0) & attacks_from_bishop(s2, 0)) | s1 | s2;
        between[s1][s2] = attacks_from_bishop(s1, square[s2]) & attacks_from_bishop(s2, square[s1]);
        }
      else if (pseudo_attack[rook][s1] & s2)
        {
        line[s1][s2] = (attacks_from_rook(s1, 0) & attacks_from_rook(s2, 0)) | s1 | s2;
        between[s1][s2] = attacks_from_rook(s1, square[s2]) & attacks_from_rook(s2, square[s1]);
        }
      }
    }
  }


e_square least_significant_bit(bitboard b)
  {
  return bit_scan_forward_table[bit_scan_forward_index(b)];
  }

e_square pop_least_significant_bit(bitboard& b)
  {
  bitboard bb = b;
  b = bb & (bb - 1);
  return bit_scan_forward_table[bit_scan_forward_index(bb)];
  }

e_square most_significant_bit(bitboard b)
  {
  unsigned b32;
  int result = 0;

  if (b > 0xFFFFFFFF)
    {
    b >>= 32;
    result = 32;
    }

  b32 = unsigned(b);

  if (b32 > 0xFFFF)
    {
    b32 >>= 16;
    result += 16;
    }

  if (b32 > 0xFF)
    {
    b32 >>= 8;
    result += 8;
    }

  return e_square(result + bit_scan_reverse_most_significant_1bit_table[b32]);
  }