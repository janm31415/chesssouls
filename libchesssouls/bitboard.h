#pragma once

#include "libchesssouls_api.h"
#include "types.h"

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

const bitboard fileA = 0x0101010101010101;
const bitboard fileB = fileA << 1;
const bitboard fileC = fileA << 2;
const bitboard fileD = fileA << 3;
const bitboard fileE = fileA << 4;
const bitboard fileF = fileA << 5;
const bitboard fileG = fileA << 6;
const bitboard fileH = fileA << 7;

const bitboard rank1 = 0xff;
const bitboard rank2 = rank1 << 8;
const bitboard rank3 = rank1 << 16;
const bitboard rank4 = rank1 << 24;
const bitboard rank5 = rank1 << 32;
const bitboard rank6 = rank1 << 40;
const bitboard rank7 = rank1 << 48;
const bitboard rank8 = rank1 << 56;

extern bitboard square[nr_squares];
extern bitboard file[nr_files];
extern bitboard rank[nr_ranks];
extern bitboard adjacent_files[nr_files];

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
