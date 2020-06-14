#pragma once
#include <stdint.h>
#include <cassert>

typedef uint64_t bitboard;

/*
bit 0-5: destination square
bit 6-11: source square
bit 12-13: promotion piece type (knight-2==0 to queen-2==3)
bit 14-15: special move flag:
             normal (0) 
             promotion (1)
             en passant (2)
             castling (3)
*/
typedef uint16_t move;

constexpr move move_none = 0;
constexpr move move_null = 65;

enum e_movetype
  {
  normal = 0,
  promotion = 1<<14,
  enpassant = 2<<14,
  castling = 3<<14
  };

#define nr_squares 64
#define nr_files 8
#define nr_ranks 8
#define nr_piecetype 7
#define nr_pieces 16
#define nr_color 2
#define max_moves 256

enum e_square
  {
  sq_a1, sq_b1, sq_c1, sq_d1, sq_e1, sq_f1, sq_g1, sq_h1,
  sq_a2, sq_b2, sq_c2, sq_d2, sq_e2, sq_f2, sq_g2, sq_h2,
  sq_a3, sq_b3, sq_c3, sq_d3, sq_e3, sq_f3, sq_g3, sq_h3,
  sq_a4, sq_b4, sq_c4, sq_d4, sq_e4, sq_f4, sq_g4, sq_h4,
  sq_a5, sq_b5, sq_c5, sq_d5, sq_e5, sq_f5, sq_g5, sq_h5,
  sq_a6, sq_b6, sq_c6, sq_d6, sq_e6, sq_f6, sq_g6, sq_h6,
  sq_a7, sq_b7, sq_c7, sq_d7, sq_e7, sq_f7, sq_g7, sq_h7,
  sq_a8, sq_b8, sq_c8, sq_d8, sq_e8, sq_f8, sq_g8, sq_h8,
  sq_end, sq_none,
  sq_delta_up = 8,
  sq_delta_down = -8,
  sq_delta_left = -1,
  sq_delta_right = 1,
  sq_delta_up_up = 16,
  sq_delta_down_down = -16,
  sq_delta_up_left = 7,
  sq_delta_up_right = 9,
  sq_delta_down_left = -9,
  sq_delta_down_right = -7
  };

enum e_file
  {
  file_a, file_b, file_c, file_d, file_e, file_f, file_g, file_h, file_end
  };

enum e_rank
  {
  rank_1, rank_2, rank_3, rank_4, rank_5, rank_6, rank_7, rank_8, rank_end
  };

enum e_piecetype
  {
  no_piecetype, pawn, knight, bishop, rook, queen, king, piecetype_end, all_pieces=0
  };

enum e_piece
  {
  no_piece,
  white_pawn, white_knight, white_bishop, white_rook, white_queen, white_king,
  black_pawn=9, black_knight, black_bishop, black_rook, black_queen, black_king
  };

enum e_color
  {
  white, black, color_end
  };

inline e_color operator ~ (e_color c)
  {
  return e_color(c^black);
  }

enum e_positive_ray
  {
  north_west, north, north_east, east
  };

enum e_negative_ray
  {
  south_east=4,south, south_west, west
  };

#define ENABLE_BASE_OPERATORS_ON(T)                                         \
inline T operator+(const T d1, const T d2) { return T(int(d1) + int(d2)); } \
inline T operator-(const T d1, const T d2) { return T(int(d1) - int(d2)); } \
inline T operator*(int i, const T d) { return T(i * int(d)); }              \
inline T operator*(const T d, int i) { return T(int(d) * i); }              \
inline T operator-(const T d) { return T(-int(d)); }                        \
inline T& operator+=(T& d1, const T d2) { return d1 = d1 + d2; }            \
inline T& operator-=(T& d1, const T d2) { return d1 = d1 - d2; }            \
inline T& operator*=(T& d, int i) { return d = T(int(d) * i); }

#define ENABLE_FULL_OPERATORS_ON(T)                                         \
ENABLE_BASE_OPERATORS_ON(T)                                                 \
inline T& operator++(T& d) { return d = T(int(d) + 1); }                    \
inline T& operator--(T& d) { return d = T(int(d) - 1); }                    \
inline T operator/(const T d, int i) { return T(int(d) / i); }              \
inline T& operator/=(T& d, int i) { return d = T(int(d) / i); }

ENABLE_FULL_OPERATORS_ON(e_square)
ENABLE_FULL_OPERATORS_ON(e_file)
ENABLE_FULL_OPERATORS_ON(e_rank)
ENABLE_FULL_OPERATORS_ON(e_piecetype)
ENABLE_FULL_OPERATORS_ON(e_color)
ENABLE_FULL_OPERATORS_ON(e_piece)

inline e_color color_of(e_piece p)
  {
  assert(p != no_piece);
  return e_color(p >> 3);
  }

inline e_piecetype type_of(e_piece p)
  {
  return e_piecetype(p & 7);
  }

inline e_square make_square(e_file f, e_rank r) 
  {
  return e_square((r << 3) | f);
  }

inline e_piece make_piece(e_color c, e_piecetype pt)
  {
  return e_piece((c << 3) | pt);
  }

inline e_file file_of(e_square s) 
  {
  return e_file(s & 7);
  }

inline e_rank rank_of(e_square s) 
  {
  return e_rank(s >> 3);
  }

inline e_square from_square(move m)
  {
  return e_square((m >> 6) & 63);
  }

inline e_square to_square(move m)
  {
  return e_square(m & 63);
  }

inline e_movetype type_of(move m)
  {
  return e_movetype(m & (3 << 14));
  }

inline e_piecetype promotion_type(move m)
  {
  return e_piecetype(((m >> 12) & 3) + 2);
  }

inline move make_move(e_square from, e_square to)
  {
  return move(to | (from << 6));
  }

template <e_movetype T>
inline move make(e_square from, e_square to, e_piecetype pt = knight)
  {
  return move(to | (from << 6) | T | ((pt - knight) << 12));
  }

inline bool is_ok(move m)
  {
  return from_square(m) != to_square(m);
  }

inline bool is_ok(e_square s)
  {
  return s >= sq_a1 && s <= sq_h8;
  }

/*
Relative rank returns the input rank when the input color is white,
or the inverted rank when the input color is black.
*/
inline e_rank relative_rank(e_color c, e_rank r)
  {
  return e_rank(r ^ (c * 7));
  }

inline e_square relative_square(e_color c, e_square s)
  {
  return e_square(s ^ (c * 56));
  }