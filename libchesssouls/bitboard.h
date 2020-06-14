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

extern bitboard rook_masks[nr_squares];
extern bitboard rook_magics[nr_squares];
extern bitboard* rook_attacks[nr_squares];
extern unsigned rook_shifts[nr_squares];

extern bitboard bishop_masks[nr_squares];
extern bitboard bishop_magics[nr_squares];
extern bitboard* bishop_attacks[nr_squares];
extern unsigned bishop_shifts[nr_squares];

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

inline bitboard& operator |= (bitboard b, e_square s)
  {
  return b |= square[s];
  }

inline bitboard& operator ^= (bitboard b, e_square s)
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

/// Functions for computing sliding attack bitboards. Function attacks() takes
/// a square and a bitboard of occupied squares as input, and returns a bitboard
/// representing all squares attacked by Pt (bishop or rook) on the given square.
template<e_piecetype Pt>
inline unsigned magic_index(e_square s, bitboard occ) {

  bitboard* const masks = Pt == rook ? rook_masks : bishop_masks;
  bitboard* const magics = Pt == rook ? rook_magics : bishop_magics;
  unsigned* const shifts = Pt == rook ? rook_shifts : bishop_shifts;

  return unsigned(((occ & masks[s]) * magics[s]) >> shifts[s]);
  }

template<e_piecetype Pt>
inline bitboard attacks_bb(e_square s, bitboard occ) 
  {
  return (Pt == rook ? rook_attacks : bishop_attacks)[s][magic_index<Pt>(s, occ)];
  }

inline bitboard rank_bb(e_square s) 
  {
  return rank[rank_of(s)];
  }

inline bitboard file_bb(e_square s)
  {
  return file[file_of(s)];
  }