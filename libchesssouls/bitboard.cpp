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

bitboard rook_masks[nr_squares];
bitboard rook_magics[nr_squares];
bitboard* rook_attacks[nr_squares];
unsigned rook_shifts[nr_squares];

bitboard bishop_masks[nr_squares];
bitboard bishop_magics[nr_squares];
bitboard* bishop_attacks[nr_squares];
unsigned bishop_shifts[nr_squares];

namespace
  {
  //Bit scan:
  //https://www.chessprogramming.org/BitScan

  const uint64_t DeBruijn_64 = 0x3F79D71B4CB0A89ULL;
  e_square bit_scan_forward_table[nr_squares];
  inline unsigned int bit_scan_forward_index(bitboard b)
    {
    b ^= (b - 1);
    return (b * DeBruijn_64) >> 58;
    }

  bitboard rook_table[0x19000]; // Storage space for rook attacks
  bitboard bishop_table[0x1480];  // Storage space for bishop attacks
  
  typedef unsigned (Fn)(e_square, bitboard);

  bitboard sliding_attack(e_square deltas[], e_square sq, bitboard occupied) 
    {
    bitboard attack = 0;

    for (int i = 0; i < 4; ++i)
      for (e_square s = sq + deltas[i]; is_ok(s) && square_distance(s, s - deltas[i]) == 1; s += deltas[i])
      {
      attack |= s;

      if (occupied & s)
        break;
      }

    return attack;
    }


  // init_magics() computes all rook and bishop attacks at startup. Magic
  // bitboards are used to look up attacks of sliding pieces. As a reference see
  // chessprogramming.wikispaces.com/Magic+bitboards. In particular, here we
  // use the so called "fancy" approach.

  void init_magics(bitboard table[], bitboard* attacks[], bitboard magics[],
    bitboard masks[], unsigned shifts[], e_square deltas[], Fn index) 
    {

    int MagicBoosters[][nr_ranks] = { {  969, 1976, 2850,  542, 2069, 2852, 1708,  164 },
                                     { 3101,  552, 3555,  926,  834,   26, 2131, 1117 } };

    RKISS rk;
    bitboard occupancy[4096], reference[4096], edges, b;
    int i, size, booster;

    // attacks[s] is a pointer to the beginning of the attacks table for square 's'
    attacks[sq_a1] = table;

    for (e_square s = sq_a1; s <= sq_h8; ++s)
      {
      // Board edges are not considered in the relevant occupancies
      edges = ((rank1 | rank8) & ~rank_bb(s)) | ((fileA | fileH) & ~file_bb(s));

      // Given a square 's', the mask is the bitboard of sliding attacks from
      // 's' computed on an empty board. The index must be big enough to contain
      // all the attacks for each possible subset of the mask and so is 2 power
      // the number of 1s of the mask. Hence we deduce the size of the shift to
      // apply to the 64 or 32 bits word to get the index.
      masks[s] = sliding_attack(deltas, s, 0) & ~edges;
      shifts[s] = 64 - __popcnt64(masks[s]);

      // Use Carry-Rippler trick to enumerate all subsets of masks[s] and
      // store the corresponding sliding attack bitboard in reference[].
      b = size = 0;
      do {
        occupancy[size] = b;
        reference[size] = sliding_attack(deltas, s, b);

        size++;
        b = (b - masks[s]) & masks[s];
        } while (b);

        // Set the offset for the table of the next square. We have individual
        // table sizes for each square with "Fancy Magic bitboards".
        if (s < sq_h8)
          attacks[s + 1] = attacks[s] + size;

        booster = MagicBoosters[1][rank_of(s)];

        // Find a magic for square 's' picking up an (almost) random number
        // until we find the one that passes the verification test.
        do {
          do
            magics[s] = rk.magic_rand<bitboard>(booster);
          while (__popcnt64((magics[s] * masks[s]) >> 56) < 6);

          std::memset(attacks[s], 0, size * sizeof(bitboard));

          // A good magic must map every possible occupancy to an index that
          // looks up the correct sliding attack in the attacks[s] database.
          // Note that we build up the database for square 's' as a side
          // effect of verifying the magic.
          for (i = 0; i < size; ++i)
            {
            bitboard& attack = attacks[s][index(s, occupancy[i])];

            if (attack && attack != reference[i])
              break;

            assert(reference[i]);

            attack = reference[i];
            }
          } while (i < size);
      }
    }
  }

void init_bitboards()
  {
  for (auto s = sq_a1; s < sq_end; ++s)
    {
    square[s] = (uint64_t)1 << s;
    bit_scan_forward_table[bit_scan_forward_index(square[s])] = s;
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

  e_square rook_deltas[] = { sq_delta_up,  sq_delta_right,  sq_delta_down,  sq_delta_left };
  e_square bishop_deltas[] = { sq_delta_up_right, sq_delta_down_right, sq_delta_down_left, sq_delta_up_left };

  init_magics(rook_table, rook_attacks, rook_magics, rook_masks, rook_shifts, rook_deltas, magic_index<rook>);
  init_magics(bishop_table, bishop_attacks, bishop_magics, bishop_masks, bishop_shifts, bishop_deltas, magic_index<bishop>);

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