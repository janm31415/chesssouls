#pragma once

#include "libchesssouls_api.h"
#include "types.h"
#include <stdlib.h>

/*
 Rank
 ||
 \/
  8
  7
  6
  5
  4
  3
  2
  1
    A B C D E F G H  <= File
*/

constexpr bitboard fileA = 0x0101010101010101;
constexpr bitboard fileB = fileA << 1;
constexpr bitboard fileC = fileA << 2;
constexpr bitboard fileD = fileA << 3;
constexpr bitboard fileE = fileA << 4;
constexpr bitboard fileF = fileA << 5;
constexpr bitboard fileG = fileA << 6;
constexpr bitboard fileH = fileA << 7;

constexpr bitboard rank1 = 0xff;
constexpr bitboard rank2 = rank1 << 8;
constexpr bitboard rank3 = rank1 << 16;
constexpr bitboard rank4 = rank1 << 24;
constexpr bitboard rank5 = rank1 << 32;
constexpr bitboard rank6 = rank1 << 40;
constexpr bitboard rank7 = rank1 << 48;
constexpr bitboard rank8 = rank1 << 56;

extern bitboard square[nr_squares];
extern bitboard file[nr_files];
extern bitboard rank[nr_ranks];
extern bitboard adjacent_files[nr_files];
extern bitboard step_attacks[nr_pieces][nr_squares];
extern int square_distance_table[nr_squares][nr_squares];

extern bitboard ray_attack[8][nr_squares];
extern bitboard line[nr_squares][nr_squares];
extern bitboard between[nr_squares][nr_squares];
extern bitboard castling_path[9];
extern bitboard pseudo_attack[nr_piecetype][nr_squares];

LIB_CHESSSOULS_API void init_bitboards();

inline bitboard operator & (bitboard b, e_square s)
  {
  return b & square[s];
  }

inline bitboard operator | (bitboard b, e_square s)
  {
  return b | square[s];
  }

inline bitboard operator ^ (bitboard b, e_square s)
  {
  return b ^ square[s];
  }

inline bitboard& operator |= (bitboard& b, e_square s)
  {
  return b |= square[s];
  }

inline bitboard& operator ^= (bitboard& b, e_square s)
  {
  return b ^= square[s];
  }

inline bool more_than_one(bitboard b)
  {
  return b & (b - 1);
  }

// shift_bb() moves bitboard one step along direction delta. Mainly for pawns, see generate_pawn_moves in movegen.cpp
template <e_square delta>
inline bitboard shift_bb(bitboard b)
  {
  return  delta == sq_delta_up ? b << 8 : delta == sq_delta_down ? b >> 8
    : delta == sq_delta_up_right ? (b & ~fileH) << 9 : delta == sq_delta_down_right ? (b & ~fileH) >> 7
    : delta == sq_delta_up_left ? (b & ~fileA) << 7 : delta == sq_delta_down_left ? (b & ~fileA) >> 9
    : 0;
  }

e_square most_significant_bit(bitboard b);
e_square least_significant_bit(bitboard b);
e_square pop_least_significant_bit(bitboard& b);

inline int file_distance(e_square s1, e_square s2) 
  {
  return abs(file_of(s1) - file_of(s2));
  }

inline int rank_distance(e_square s1, e_square s2) 
  {
  return abs(rank_of(s1) - rank_of(s2));
  }

inline int square_distance(e_square s1, e_square s2) 
  {
  return square_distance_table[s1][s2];
  }

inline bitboard attacks_from_pawn(e_square s, e_color c)
  {
  return step_attacks[make_piece(c, pawn)][s];
  }

inline bitboard attacks_positive_ray_direction(e_positive_ray dir, e_square s, bitboard occ)
  {
  bitboard attacks = ray_attack[dir][s];
  bitboard blocker = attacks & occ;
  if (blocker)
    {
    e_square sq = most_significant_bit(blocker);
    attacks ^= ray_attack[dir][sq];
    }
  return attacks;
  }

inline bitboard attacks_negative_ray_direction(e_negative_ray dir, e_square s, bitboard occ)
  {
  bitboard attacks = ray_attack[dir][s];
  bitboard blocker = attacks & occ;
  if (blocker)
    {
    e_square sq = least_significant_bit(blocker);
    attacks ^= ray_attack[dir][sq];
    }
  return attacks;
  }

inline bitboard attacks_from_rook(e_square s, bitboard occ) 
  {
  return attacks_positive_ray_direction(north, s, occ) |
    attacks_positive_ray_direction(east, s, occ) |
    attacks_negative_ray_direction(south, s, occ) |
    attacks_negative_ray_direction(west, s, occ);
  } 

inline bitboard attacks_from_bishop(e_square s, bitboard occ)
  {
  return attacks_positive_ray_direction(north_west, s, occ) |
    attacks_positive_ray_direction(north_east, s, occ) |
    attacks_negative_ray_direction(south_west, s, occ) |
    attacks_negative_ray_direction(south_east, s, occ);
  }

inline bitboard rank_bb(e_square s) 
  {
  return rank[rank_of(s)];
  }

inline bitboard file_bb(e_square s)
  {
  return file[file_of(s)];
  }

inline bool aligned(e_square s1, e_square s2, e_square s3) 
  {
  return line[s1][s2] & s3;
  }