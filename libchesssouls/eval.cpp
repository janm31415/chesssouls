#include "eval.h"
#include "position.h"

#include "pawn.h"
#include "king.h"
#include "eval_table.h"

namespace
  {
  int pawn_pcsq_mg[nr_squares] = {
    -15, -5, 0, 5, 5, 0, -5, -15,
    -15, -5, 0, 5, 5, 0, -5, -15,
    -15, -5, 0, 15, 15, 0, -5, -15,
    -15, -5, 0, 25, 25, 0, -5, -15,
    -15, -5, 0, 15, 15, 0, -5, -15,
    -15, -5, 0, 5, 5, 0, -5, -15,
    -15, -5, 0, 5, 5, 0, -5, -15,
    -15, -5, 0, 5, 5, 0, -5, -15,
    };

  int pawn_pcsq_eg[nr_squares] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
    };

  int knight_pcsq_mg[nr_squares] = {
    -50, -40, -30, -25, -25, -30, -40, -50,
    -35, -25, -15, -10, -10, -15, -25, -35,
    -20, -10, 0, 5, 5, 0, -10, -20,
    -10, 0, 10, 15, 15, 10, 0, -10,
    -5, 5, 15, 20, 20, 15, 5, -5,
    -5, 5, 15, 20, 20, 15, 5, -5,
    -20, -10, 0, 5, 5, 0, -10, -20,
    -135, -25, -15, -10, -10, -15, -25, -135
    };

  int knight_pcsq_eg[nr_squares] = {
    -40, -30, -20, -15, -15, -20, -30, -40,
    -30, -20, -10, -5, -5, -10, -20, -30,
    -20, -10, 0, 5, 5, 0, -10, -20,
    -15, -5, 5, 10, 10, 5, -5, -15,
    -15, -5, 5, 10, 10, 5, -5, -15,
    -20, -10, 0, 5, 5, 0, -10, -20,
    -30, -20, -10, -5, -5, -10, -20, -30,
    -40, -30, -20, -15, -15, -20, -30, -40
    };

  int bishop_pcsq_mg[nr_squares] = {
    -18, -18, -16, -14, -14, -16, -18, -18,
    -8, 0, -2, 0, 0, -2, 0, -8,
    -6, -2, 4, 2, 2, 4, -2, -6,
    -4, 0, 2, 8, 8, 2, 0, -4,
    -4, 0, 2, 8, 8, 2, 0, -4,
    -6, -2, 4, 2, 2, 4, -2, -6,
    -8, 0, -2, 0, 0, -2, 0, -8,
    -8, -8, -6, -4, -4, -6, -8, -8
    };

  int bishop_pcsq_eg[nr_squares] = {
    -18, -12, -9, -6, -6, -9, -12, -18,
    -12, -6, -3, 0, 0, -3, -6, -12,
    -9, -3, 0, 3, 3, 0, -3, -9,
    -6, 0, 3, 6, 6, 3, 0, -6,
    -6, 0, 3, 6, 6, 3, 0, -6,
    -9, -3, 0, 3, 3, 0, -3, -9,
    -12, -6, -3, 0, 0, -3, -6, -12,
    -18, -12, -9, -6, -6, -9, -12, -18
    };

  int rook_pcsq_mg[nr_squares] = {
    -6, -3, 0, 3, 3, 0, -3, -6,
    -6, -3, 0, 3, 3, 0, -3, -6,
    -6, -3, 0, 3, 3, 0, -3, -6,
    -6, -3, 0, 3, 3, 0, -3, -6,
    -6, -3, 0, 3, 3, 0, -3, -6,
    -6, -3, 0, 3, 3, 0, -3, -6,
    -6, -3, 0, 3, 3, 0, -3, -6,
    -6, -3, 0, 3, 3, 0, -3, -6
    };

  int rook_pcsq_eg[nr_squares] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
    };

  int queen_pcsq_mg[nr_squares] = {
    -5, -5, -5, -5, -5, -5, -5, -5,
    0, 0, 1, 1, 1, 1, 0, 0,
    0, 0, 1, 2, 2, 1, 0, 0,
    0, 0, 2, 3, 3, 2, 0, 0,
    0, 0, 2, 3, 3, 2, 0, 0,
    0, 0, 1, 2, 2, 1, 0, 0,
    0, 0, 1, 1, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
    };

  int queen_pcsq_eg[nr_squares] = {
    -24, -16, -12, -8, -8, -12, -16, -24,
    -16, -8, -4, 0, 0, -4, -8, -16,
    -12, -4, 0, 4, 4, 0, -4, -12,
    -8, 0, 4, 8, 8, 4, 0, -8,
    -8, 0, 4, 8, 8, 4, 0, -8,
    -12, -4, 0, 4, 4, 0, -4, -12,
    -16, -8, -4, 0, 0, -4, -8, -16,
    -24, -16, -12, -8, -8, -12, -16, -24
    };

  int king_pcsq_mg[nr_squares] = {
    40, 50, 30, 10, 10, 30, 50, 40,
    30, 40, 20, 0, 0, 20, 40, 30,
    10, 20, 0, -20, -20, 0, 20, 10,
    0, 10, -10, -30, -30, -10, 10, 0,
    -10, 0, -20, -40, -40, -20, 0, -10,
    -20, -10, -30, -50, -50, -30, -10, -20,
    -30, -20, -40, -60, -60, -40, -20, -30,
    -40, -30, -50, -70, -70, -50, -30, -40
    };

  int king_pcsq_eg[nr_squares] = {
    -72, -48, -36, -24, -24, -36, -48, -72,
    -48, -24, -12, 0, 0, -12, -24, -48,
    -36, -12, 0, 12, 12, 0, -12, -36,
    -24, 0, 12, 24, 24, 12, 0, -24,
    -24, 0, 12, 24, 24, 12, 0, -24,
    -36, -12, 0, 12, 12, 0, -12, -36,
    -48, -24, -12, 0, 0, -12, -24, -48,
    -72, -48, -36, -24, -24, -36, -48, -72
    };

  int total_piece_score_opening;
  }

int pcsq[2][7][nr_squares];

int piece_value[7];
int piece_value_see[7];

int piece_to_attack_value[7] = {0, 0, 20, 20, 40, 80, 10};
int nr_of_attackers_to_weight[] = { 0, 0, 50, 75, 88, 94, 97, 99, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100
, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 };

int dist_bonus[64][64];
int qk_dist[64][64];
int rk_dist[64][64];
int bk_dist[64][64];
int nk_dist[64][64];

uint8_t squares_near_king[2][64][64];

int log_table[141];

struct evaluation
  {
  int mobility[2];
  int attack[2];
  int attack_value[2];
  int trapped_opening[2];
  int trapped_endgame[2];
  int attack_pattern[2];
  int pawn_eval[2];
  int white_bishops;
  int black_bishops;
  int16_t WhiteKingShield;
  int16_t BlackKingShield;
  int WhiteKingSafety;
  int BlackKingSafety;
  int TropismToWhiteKing;
  int TropismToBlackKing;
  int AttackWhiteKingScore;
  int AttackBlackKingScore;
  int WhiteMobilityScore;
  int BlackMobilityScore;
  int WhiteBonus;
  int BlackBonus;
  };

evaluation eval_data;

int pawn_rank_shift[2][10] = { { 0, 3, 6, 9, 12, 15, 18, 21, 24, 27}, { 30, 33, 36, 39, 42, 45, 48, 51, 54, 57} };
uint64_t PR;

int diag_nw[64] = {
  0, 1, 2, 3, 4, 5, 6, 7, 
  1, 2, 3, 4, 5, 6, 7, 8, 
  2, 3, 4, 5, 6, 7, 8, 9, 
  3, 4, 5, 6, 7, 8, 9, 10, 
  4, 5, 6, 7, 8, 9, 10, 11, 
  5, 6, 7, 8, 9, 10, 11, 12, 
  6, 7, 8, 9, 10, 11, 12, 13, 
  7, 8, 9, 10, 11, 12, 13, 14
  };

int diag_ne[64] = {
  7, 6, 5, 4, 3, 2, 1, 0, 
  8, 7, 6, 5, 4, 3, 2, 1,
  9, 8, 7, 6, 5, 4, 3, 2, 
  10, 9, 8, 7, 6, 5, 4, 3, 
  11, 10, 9, 8, 7, 6, 5, 4,
  12, 11, 10, 9, 8, 7, 6, 5, 
  13, 12, 11, 10, 9, 8, 7, 6,
  14, 13, 12, 11, 10, 9, 8, 7
  };

void set_pawn_rank(int color_bit, int f, uint64_t r)
  {
  int shift = pawn_rank_shift[color_bit][f];
  uint64_t mask = uint64_t(7) << shift;
  PR = (PR & ~mask) | (r << shift);
  }

int get_pawn_rank(int color_bit, int f)
  {
  int shift = pawn_rank_shift[color_bit][f];
  uint64_t mask = uint64_t(7) << shift;
  return (int)((PR & mask) >> shift);
  }

int bonus_dia_distance[15] = { 5, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


void init_log_table()
  {
  log_table[0] = -1;
  log_table[1] = 0;
  for (int i = 2; i < 141; ++i)
    log_table[i] = int(floor(log2(double(i)))) * mobility_bonus;
  }

void init_dist()
  {
  for (int i = 0; i < 64; ++i)
    {
    int col_i = (int)file_of((e_square)i);
    int row_i = (int)rank_of((e_square)i);
    for (int j = 0; j < 64; ++j)
      {
      int col_j = (int)file_of((e_square)j);
      int row_j = (int)rank_of((e_square)j);
      dist_bonus[i][j] = 14 - (std::abs(col_i - col_j) + std::abs(row_i - row_j));
      qk_dist[i][j] = (dist_bonus[i][j] * 5) / 2;
      rk_dist[i][j] = dist_bonus[i][j] / 2;
      nk_dist[i][j] = dist_bonus[i][j];
      bk_dist[i][j] = rk_dist[i][j];
      bk_dist[i][j] += bonus_dia_distance[std::abs(diag_ne[i] - diag_ne[j])];
      bk_dist[i][j] += bonus_dia_distance[std::abs(diag_nw[i] - diag_nw[j])];
      }
    }
  }

void init_squares_near_king()
  {
  memset(squares_near_king, 0, sizeof(squares_near_king));
  int steps[8] = { -9, -8, -7, -1, 9, 8, 7, 1 };
  for (int i = 0; i < 64; ++i)
    {
    for (int j = 0; j < 64; ++j)
      {
      for (int d = 0; d < 8; ++d)
        {        
        if (i == j)
          {
          squares_near_king[0][i][j] = 1;
          squares_near_king[1][i][j] = 1;
          }        
        if (i == j + steps[d])
          {
          if (square_distance(e_square(i), e_square(j)) < 3)
            {
            squares_near_king[0][i][j] = 1;
            squares_near_king[1][i][j] = 1;
            }
          }
        if (i == j + steps[d] - 8)
          {
          if (square_distance(e_square(i), e_square(j)) < 3)
            squares_near_king[0][i][j] = 1;
          }
        if (i == j + steps[d] + 8)
          {
          if (square_distance(e_square(i), e_square(j)) < 3)
            squares_near_king[1][i][j] = 1;
          }
        }
      }
    }
  }

void init_eval()
  {
  for (e_square s1 = sq_a1; s1 <= sq_h8; ++s1)
    {
    uint8_t neg = ~((uint8_t)s1) & 63;
    pcsq[white][pawn][s1] = make_score(pawn_pcsq_mg[s1], pawn_pcsq_eg[s1]);
    pcsq[white][knight][s1] = make_score(knight_pcsq_mg[s1], knight_pcsq_eg[s1]);
    pcsq[white][bishop][s1] = make_score(bishop_pcsq_mg[s1], bishop_pcsq_eg[s1]);
    pcsq[white][rook][s1] = make_score(rook_pcsq_mg[s1], rook_pcsq_eg[s1]);
    pcsq[white][queen][s1] = make_score(queen_pcsq_mg[s1], queen_pcsq_eg[s1]);
    pcsq[white][king][s1] = make_score(king_pcsq_mg[s1], king_pcsq_eg[s1]);
    pcsq[black][pawn][neg] = make_score(pawn_pcsq_mg[s1], pawn_pcsq_eg[s1]);
    pcsq[black][knight][neg] = make_score(knight_pcsq_mg[s1], knight_pcsq_eg[s1]);
    pcsq[black][bishop][neg] = make_score(bishop_pcsq_mg[s1], bishop_pcsq_eg[s1]);
    pcsq[black][rook][neg] = make_score(rook_pcsq_mg[s1], rook_pcsq_eg[s1]);
    pcsq[black][queen][neg] = make_score(queen_pcsq_mg[s1], queen_pcsq_eg[s1]);
    pcsq[black][king][neg] = make_score(king_pcsq_mg[s1], king_pcsq_eg[s1]);
    }
  piece_value[pawn] = make_score(pawn_value_mg, pawn_value_eg);
  piece_value[knight] = make_score(knight_value_mg, knight_value_eg);
  piece_value[bishop] = make_score(bishop_value_mg, bishop_value_eg);
  piece_value[rook] = make_score(rook_value_mg, rook_value_eg);
  piece_value[queen] = make_score(queen_value_mg, queen_value_eg);
  piece_value[king] = 0;

  total_piece_score_opening = mg_value(piece_value[knight]) * 2 + mg_value(piece_value[bishop]) * 2 + mg_value(piece_value[rook]) * 2 + mg_value(piece_value[queen]);

  piece_value_see[pawn] = 100;
  piece_value_see[knight] = 300;
  piece_value_see[bishop] = 300;
  piece_value_see[rook] = 500;
  piece_value_see[queen] = 900;
  piece_value_see[king] = 10000;

  piece_to_attack_value[knight] = knight_attack_value;
  piece_to_attack_value[bishop] = bishop_attack_value;
  piece_to_attack_value[rook] = rook_attack_value;
  piece_to_attack_value[queen] = queen_attack_value;

  init_dist();
  init_squares_near_king();
  init_log_table();
  }

int eval_white_pawn(e_square sq)
  {
  int ret = 0;
  int f = (int)file_of(sq);
  int r = (int)rank_of(sq);

  /* if there's a pawn behind this one, it's doubled */  
  if (get_pawn_rank(0, f + 1) < r)
    ret -= doubled_pawn_penalty;

  /* if there aren't any friendly pawns on either side of
  this one, it's isolated */  
  if ((get_pawn_rank(0, f) == 7) && (get_pawn_rank(0, f + 2) == 7))
    ret -= isolated_pawn_penalty;

  /* if it's not isolated, it might be backwards */  
  else if ((get_pawn_rank(0, f) > r) && (get_pawn_rank(0, f + 2) > r))
    ret -= backwards_pawn_penalty;

  /* add a bonus if the pawn is passed */  
  if ((get_pawn_rank(1, f) <= r) && (get_pawn_rank(1, f + 1) <= r) && (get_pawn_rank(1, f + 2) <= r))
    ret += r * passed_pawn_bonus;
  return ret;
  }

int eval_black_pawn(e_square sq)
  {
  int ret = 0;
  int f = (int)file_of(sq);
  int r = (int)rank_of(sq);

  /* if there's a pawn behind this one, it's doubled */  
  if (get_pawn_rank(1, f + 1) > r)
    ret -= doubled_pawn_penalty;

  /* if there aren't any friendly pawns on either side of
  this one, it's isolated */  
  if ((get_pawn_rank(1, f) == 0) && (get_pawn_rank(1, f + 2) == 0))
    ret -= isolated_pawn_penalty;

  /* if it's not isolated, it might be backwards */  
  else if ((get_pawn_rank(1, f) < r) && (get_pawn_rank(1, f + 2) < r))
    ret -= backwards_pawn_penalty;

  /* add a bonus if the pawn is passed */  
  if ((get_pawn_rank(0, f) >= r) && (get_pawn_rank(0, f + 1) >= r) && (get_pawn_rank(0, f + 2) >= r))
    ret += (7 - r) * passed_pawn_bonus;
  return ret;
  }

/* eval_wkp(f) evaluates the White King Pawn on file f */

int16_t eval_wkp(int f)
  {
  int16_t r = 0;

  if (get_pawn_rank(0, f) == 1);  /* pawn hasn't moved */
  else if (get_pawn_rank(0, f) == 2)
    r -= 10;  /* pawn moved one square */
  else if (get_pawn_rank(0, f) != 7)
    r -= 20;  /* pawn moved more than one square */
  else
    r -= 25;  /* no pawn on this file */

  if (get_pawn_rank(1, f) == 0)
    r -= 15;  /* no enemy pawn */
  else if (get_pawn_rank(1, f) == 2)
    r -= 10;  /* enemy pawn on the 3rd rank */
  else if (get_pawn_rank(1, f) == 3)
    r -= 5;   /* enemy pawn on the 4th rank */

  return r;
  }

int16_t eval_bkp(int f)
  {
  int16_t r = 0;

  if (get_pawn_rank(1, f) == 6);
  else if (get_pawn_rank(1, f) == 5)
    r -= 10;
  else if (get_pawn_rank(1, f) != 0)
    r -= 20;
  else
    r -= 25;

  if (get_pawn_rank(0, f) == 7)
    r -= 15;
  else if (get_pawn_rank(0, f) == 5)
    r -= 10;
  else if (get_pawn_rank(0, f) == 4)
    r -= 5;
  return r;
  }

int16_t eval_white_king_shield(e_square sq)
  {
  int16_t r = 0;
  int f = (int)file_of(sq);
  if (f < 3)
    {
    r += eval_wkp(1);
    r += eval_wkp(2);
    r += eval_wkp(3) / 2;
    }
  else if (f > 4)
    {
    r += eval_wkp(8);
    r += eval_wkp(7);
    r += eval_wkp(6) / 2;
    }
  else
    {
    for (int i = f; i <= f + 2; ++i)
      if ((get_pawn_rank(0, i) == 7) && (get_pawn_rank(1, i) == 0))
        r -= 10;
    }

  return r;
  }

int16_t eval_black_king_shield(e_square sq)
  {
  int16_t r = 0;
  int f = (int)file_of(sq);
  if (f < 3)
    {
    r += eval_bkp(1);
    r += eval_bkp(2);
    r += eval_bkp(3) / 2;
    }
  else if (f > 4)
    {
    r += eval_bkp(8);
    r += eval_bkp(7);
    r += eval_bkp(6) / 2;
    }
  else
    {
    for (int i = f; i <= f + 2; ++i)
      if ((get_pawn_rank(0, i) == 7) && (get_pawn_rank(1, i) == 0))
        r -= 10;
    }
  return r;
  }

void compute_pawn_scores(const position& pos)
  {
  bitboard white_pawns = pos.pieces(white, pawn);
  bitboard black_pawns = pos.pieces(black, pawn);

  for (int f = 0; f < 8; ++f)
    {    
    e_rank r = rank_of(least_significant_bit(white_pawns & file[f]));
    set_pawn_rank(0, f+1, r); // white
    r = rank_of(most_significant_bit(black_pawns & file[f]));
    set_pawn_rank(1, f+1, r); // black
    }

  while (white_pawns)
    {
    e_square s = pop_least_significant_bit(white_pawns);
    eval_data.pawn_eval[0] += eval_white_pawn(s);
    }
  while (black_pawns)
    {
    e_square s = pop_least_significant_bit(black_pawns);
    eval_data.pawn_eval[1] += eval_black_pawn(s);
    }
  }

void compute_king_shields(const position& pos)
  {
  // assumes pawn rank has been computed by compute_pawn_scores
  eval_data.WhiteKingShield = eval_white_king_shield(pos.king_square(white));
  eval_data.BlackKingShield = eval_black_king_shield(pos.king_square(black));
  }

void eval_pattern(const position& pos)
  {
  // trapped bishop (7th rank)

  if (((pos.piece_on(sq_a7) == white_bishop) && (pos.piece_on(sq_b6) == black_pawn)) ||
    ((pos.piece_on(sq_b8) == white_bishop) && (pos.piece_on(sq_c7) == black_pawn)))
    {
    eval_data.trapped_opening[0] -= trapped_bishop;
    eval_data.trapped_endgame[0] -= trapped_bishop;
    }
  if (((pos.piece_on(sq_h7) == white_bishop) && (pos.piece_on(sq_g6) == black_pawn)) ||
    ((pos.piece_on(sq_g8) == white_bishop) && (pos.piece_on(sq_f7) == black_pawn)))
    {
    eval_data.trapped_opening[0] -= trapped_bishop;
    eval_data.trapped_endgame[0] -= trapped_bishop;
    }
  if (((pos.piece_on(sq_a2) == black_bishop) && (pos.piece_on(sq_b3) == white_pawn)) ||
    ((pos.piece_on(sq_b1) == black_bishop) && (pos.piece_on(sq_c2) == white_pawn)))
    {
    eval_data.trapped_opening[1] -= trapped_bishop;
    eval_data.trapped_endgame[1] -= trapped_bishop;
    }
  if (((pos.piece_on(sq_h2) == black_bishop) && (pos.piece_on(sq_g3) == white_pawn)) ||
    ((pos.piece_on(sq_g1) == black_bishop) && (pos.piece_on(sq_f2) == white_pawn)))
    {
    eval_data.trapped_opening[1] -= trapped_bishop;
    eval_data.trapped_endgame[1] -= trapped_bishop;
    }

  // trapped bishop (6th rank)
  if ((pos.piece_on(sq_a6) == white_bishop) && (pos.piece_on(sq_b5) == black_pawn))
    {
    eval_data.trapped_opening[0] -= trapped_bishop;
    eval_data.trapped_endgame[0] -= trapped_bishop;
    }
  if ((pos.piece_on(sq_h6) == white_bishop) && (pos.piece_on(sq_g5) == black_pawn))
    {
    eval_data.trapped_opening[0] -= trapped_bishop;
    eval_data.trapped_endgame[0] -= trapped_bishop;
    }

  if ((pos.piece_on(sq_a3) == black_bishop) && (pos.piece_on(sq_b4) == white_pawn))
    {
    eval_data.trapped_opening[1] -= trapped_bishop;
    eval_data.trapped_endgame[1] -= trapped_bishop;
    }
  if ((pos.piece_on(sq_h3) == black_bishop) && (pos.piece_on(sq_g4) == white_pawn))
    {
    eval_data.trapped_opening[1] -= trapped_bishop;
    eval_data.trapped_endgame[1] -= trapped_bishop;
    }

  // blocked bishop
  if ((pos.piece_on(sq_d2) == white_pawn) && (pos.piece_on(sq_d3) != no_piece) && (pos.piece_on(sq_c1) == white_bishop))
    {
    eval_data.trapped_opening[0] -= trapped_bishop;
    eval_data.trapped_endgame[0] -= trapped_bishop;
    }
  if ((pos.piece_on(sq_e2) == white_pawn) && (pos.piece_on(sq_e3) != no_piece) && (pos.piece_on(sq_f1) == white_bishop))
    {
    eval_data.trapped_opening[0] -= trapped_bishop;
    eval_data.trapped_endgame[0] -= trapped_bishop;
    }

  if ((pos.piece_on(sq_d7) == black_pawn) && (pos.piece_on(sq_d6) != no_piece) && (pos.piece_on(sq_c8) == black_bishop))
    {
    eval_data.trapped_opening[1] -= trapped_bishop;
    eval_data.trapped_endgame[1] -= trapped_bishop;
    }
  if ((pos.piece_on(sq_e7) == black_pawn) && (pos.piece_on(sq_e6) != no_piece) && (pos.piece_on(sq_f8) == black_bishop))
    {
    eval_data.trapped_opening[1] -= trapped_bishop;
    eval_data.trapped_endgame[1] -= trapped_bishop;
    }

  // blocked rook
  if ((pos.piece_on(sq_c1) == white_king || pos.piece_on(sq_b1) == white_king)
    && (pos.piece_on(sq_a1) == white_rook || pos.piece_on(sq_a2) == white_rook || pos.piece_on(sq_b1) == white_rook))
    {
    eval_data.trapped_opening[0] -= blocked_rook;
    }
  if ((pos.piece_on(sq_f1) == white_king || pos.piece_on(sq_g1) == white_king)
    && (pos.piece_on(sq_h1) == white_rook || pos.piece_on(sq_h2) == white_rook || pos.piece_on(sq_g1) == white_rook))
    {
    eval_data.trapped_opening[0] -= blocked_rook;
    }
  if ((pos.piece_on(sq_c8) == black_king || pos.piece_on(sq_b8) == black_king)
    && (pos.piece_on(sq_a8) == black_rook || pos.piece_on(sq_a7) == black_rook || pos.piece_on(sq_b8) == black_rook))
    {
    eval_data.trapped_opening[1] -= blocked_rook;
    }
  if ((pos.piece_on(sq_f8) == black_king || pos.piece_on(sq_g8) == black_king)
    && (pos.piece_on(sq_h8) == black_rook || pos.piece_on(sq_h7) == black_rook || pos.piece_on(sq_g8) == black_rook))
    {
    eval_data.trapped_opening[1] -= blocked_rook;
    }
  }
/*
void eval_knight(const position& pos, e_square sq, e_color side, int side_as_bit, int xside_as_bit, e_square other_king_sq)
  {  
  bitboard my_pieces = pos.pieces(side);
  bitboard captures_or_empty_places = pos.attacks_from<knight>(sq) & ~my_pieces;
  while (captures_or_empty_places)
    {
    e_square s = pop_least_significant_bit(captures_or_empty_places);
    ++eval_data.mobility[side_as_bit];
    if (squares_near_king[xside_as_bit][(int)other_king_sq][(int)s])
      {
      ++eval_data.attack[side_as_bit];
      eval_data.attack_value[side_as_bit] += piece_to_attack_value[knight];
      }
    }
  }
*/
template <e_piecetype Pt>
void eval_piece(const position& pos, e_square sq, e_color side, int side_as_bit, int xside_as_bit, e_square other_king_sq)
  {
  bitboard my_pieces = pos.pieces(side);
  bitboard captures_or_empty_places = pos.attacks_from<Pt>(sq) & ~my_pieces;
  while (captures_or_empty_places)
    {
    e_square s = pop_least_significant_bit(captures_or_empty_places);
    ++eval_data.mobility[side_as_bit];
    if (squares_near_king[xside_as_bit][(int)other_king_sq][(int)s])
      {
      ++eval_data.attack[side_as_bit];
      eval_data.attack_value[side_as_bit] += piece_to_attack_value[Pt];
      }
    }
  }

void eval_knights(const position& pos)
  {
  bitboard white_knights = pos.pieces(white, knight);  
  while (white_knights)
    {
    e_square s = pop_least_significant_bit(white_knights);
    eval_data.TropismToBlackKing += nk_dist[(int)s][(int)pos.king_square(black)];
    eval_piece<knight>(pos, s, white, 0, 1, pos.king_square(black));
    }
  bitboard black_knights = pos.pieces(black, knight);
  while (black_knights)
    {
    e_square s = pop_least_significant_bit(black_knights);
    eval_data.TropismToWhiteKing += nk_dist[(int)s][(int)pos.king_square(white)];
    eval_piece<knight>(pos, s, black, 1, 0, pos.king_square(white));
    }
  }

void eval_bishops(const position& pos)
  {
  bitboard white_bishops = pos.pieces(white, bishop);
  while (white_bishops)
    {
    ++eval_data.white_bishops;
    e_square s = pop_least_significant_bit(white_bishops);
    eval_data.TropismToBlackKing += bk_dist[(int)s][(int)pos.king_square(black)];
    eval_piece<bishop>(pos, s, white, 0, 1, pos.king_square(black));
    }
  bitboard black_bishops = pos.pieces(black, bishop);
  while (black_bishops)
    {
    ++eval_data.black_bishops;
    e_square s = pop_least_significant_bit(black_bishops);
    eval_data.TropismToWhiteKing += bk_dist[(int)s][(int)pos.king_square(white)];
    eval_piece<bishop>(pos, s, black, 1, 0, pos.king_square(white));
    }
  }

void eval_rooks(const position& pos)
  {
  bitboard white_rooks = pos.pieces(white, rook);
  while (white_rooks)
    {
    e_square s = pop_least_significant_bit(white_rooks);
    eval_data.TropismToBlackKing += rk_dist[(int)s][(int)pos.king_square(black)];
    eval_piece<rook>(pos, s, white, 0, 1, pos.king_square(black));
    if (get_pawn_rank(0, file_of(s) + 1) == 7)
      {
      if (get_pawn_rank(1, file_of(s) + 1) == 0)
        {
        eval_data.WhiteBonus += rook_open_file_bonus;
        }
      else
        {
        eval_data.WhiteBonus += rook_semi_open_file_bonus;
        }
      }
    if (rank_of(s) == 6)
      {
      eval_data.WhiteBonus += rook_on_seventh_bonus;
      }
    }
  bitboard black_rooks = pos.pieces(black, rook);
  while (black_rooks)
    {
    e_square s = pop_least_significant_bit(black_rooks);
    eval_data.TropismToWhiteKing += rk_dist[(int)s][(int)pos.king_square(white)];
    eval_piece<rook>(pos, s, black, 1, 0, pos.king_square(white));
    if (get_pawn_rank(1, file_of(s) + 1) == 0)
      {
      if (get_pawn_rank(0, file_of(s) + 1) == 7)
        {
        eval_data.BlackBonus += rook_open_file_bonus;
        }
      else
        {
        eval_data.BlackBonus += rook_semi_open_file_bonus;
        }
      }
    if (rank_of(s) == 1)
      {
      eval_data.BlackBonus += rook_on_seventh_bonus;
      }
    }
  }

void eval_queens(const position& pos)
  {
  bitboard white_queens = pos.pieces(white, queen);
  while (white_queens)
    {
    e_square s = pop_least_significant_bit(white_queens);
    eval_data.TropismToBlackKing += qk_dist[(int)s][(int)pos.king_square(black)];
    eval_piece<queen>(pos, s, white, 0, 1, pos.king_square(black));
    }
  bitboard black_queens = pos.pieces(black, queen);
  while (black_queens)
    {
    e_square s = pop_least_significant_bit(black_queens);
    eval_data.TropismToWhiteKing += qk_dist[(int)s][(int)pos.king_square(white)];
    eval_piece<queen>(pos, s, black, 1, 0, pos.king_square(white));
    }
  }

int eval_lazy(const position& pos)
  {
  int mat_white = pos.non_pawn_material_value(white);
  int mat_black = pos.non_pawn_material_value(black);
  int pawn_white = pos.pawn_material_value(white);
  int pawn_black = pos.pawn_material_value(black);
  int pos_white = pos.positional_value(white);
  int pos_black = pos.positional_value(black);

  int mat_white_mg = mg_value(mat_white);
  int mat_black_mg = mg_value(mat_black);

  int score_white = mat_white + pawn_white + pos_white;
  int score_black = mat_black + pawn_black + pos_black;

  int score_white_mg = mg_value(score_white);
  int score_black_mg = mg_value(score_black);

  int score_white_eg = eg_value(score_white);
  int score_black_eg = eg_value(score_black);

  int ph = mat_white_mg + mat_black_mg;
  ph = (ph * 256 + total_piece_score_opening) / (2 * total_piece_score_opening);

  int sc = (ph * (score_white_mg - score_black_mg) + (256 - ph) * (score_white_eg - score_black_eg)) / 256;

  return pos.side_to_move() == white ? sc : -sc;
  }

int eval(const position& pos)
  {
  PR = 0x3fffffff;

  eval_data.mobility[0] = 0;
  eval_data.mobility[1] = 0;
  eval_data.attack[0] = 0;
  eval_data.attack[1] = 0;
  eval_data.attack_value[0] = 0;
  eval_data.attack_value[1] = 0;
  eval_data.trapped_opening[0] = 0;
  eval_data.trapped_opening[1] = 0;
  eval_data.trapped_endgame[0] = 0;
  eval_data.trapped_endgame[1] = 0;
  eval_data.attack_pattern[0] = 0;
  eval_data.attack_pattern[1] = 0;
  eval_data.pawn_eval[0] = 0;
  eval_data.pawn_eval[1] = 0;
  eval_data.TropismToWhiteKing = 0;
  eval_data.TropismToBlackKing = 0;
  eval_data.white_bishops = 0;
  eval_data.black_bishops = 0;
  eval_data.WhiteKingShield = 0;
  eval_data.BlackKingShield = 0;
  eval_data.WhiteBonus = 0;
  eval_data.BlackBonus = 0;

  compute_pawn_scores(pos);
  compute_king_shields(pos);

  eval_knights(pos);
  eval_bishops(pos);
  eval_rooks(pos);
  eval_queens(pos);

  eval_pattern(pos);

  if (eval_data.white_bishops > 1)
    eval_data.WhiteBonus += two_bishops_exist;
  if (eval_data.black_bishops > 1)
    eval_data.BlackBonus += two_bishops_exist;


  int mat_white = pos.non_pawn_material_value(white);
  int mat_black = pos.non_pawn_material_value(black);
  int pawn_white = pos.pawn_material_value(white);
  int pawn_black = pos.pawn_material_value(black);
  int pos_white = pos.positional_value(white);
  int pos_black = pos.positional_value(black);

  int mat_white_mg = mg_value(mat_white);
  int mat_black_mg = mg_value(mat_black);

  eval_data.WhiteKingSafety = ((eval_data.WhiteKingShield - eval_data.TropismToWhiteKing) * mat_black_mg) / total_piece_score_opening;
  eval_data.BlackKingSafety = ((eval_data.BlackKingShield - eval_data.TropismToBlackKing) * mat_white_mg) / total_piece_score_opening;

  eval_data.AttackWhiteKingScore = 0;//eval_data.attack_value[1] * nr_of_attackers_to_weight[eval_data.attack[1]] / 100;
  eval_data.AttackBlackKingScore = 0;//eval_data.attack_value[0] * nr_of_attackers_to_weight[eval_data.attack[0]] / 100;

  eval_data.WhiteMobilityScore = (log_table[eval_data.mobility[0]]);
  eval_data.BlackMobilityScore = (log_table[eval_data.mobility[1]]);

  eval_data.WhiteBonus += eval_data.AttackBlackKingScore + eval_data.pawn_eval[0] + eval_data.WhiteMobilityScore;
  eval_data.BlackBonus += eval_data.AttackWhiteKingScore + eval_data.pawn_eval[1] + eval_data.BlackMobilityScore;

  int score_white = mat_white + pawn_white + pos_white;
  int score_black = mat_black + pawn_black + pos_black;

  int score_white_mg = mg_value(score_white) + eval_data.WhiteBonus + eval_data.trapped_opening[0] + eval_data.WhiteKingSafety;
  int score_black_mg = mg_value(score_black) + eval_data.BlackBonus + eval_data.trapped_opening[1] + eval_data.BlackKingSafety;

  int score_white_eg = eg_value(score_white) + eval_data.WhiteBonus + eval_data.trapped_endgame[0];
  int score_black_eg = eg_value(score_black) + eval_data.BlackBonus + eval_data.trapped_endgame[1];

  int ph = mat_white_mg + mat_black_mg;
  ph = (ph * 256 + total_piece_score_opening) / (2 * total_piece_score_opening);
  
  int sc = (ph*(score_white_mg - score_black_mg) + (256 - ph)*(score_white_eg - score_black_eg)) / 256;

  return pos.side_to_move() == white ? sc : -sc;  
  }

int eval(const position& pos, int alpha, int beta)
  {
  uint64_t eval_hash = pos.compute_evaluation_zobrist_key();
  int32_t eval_white;
  bool eval_hit = find_in_eval_table(evaltable, eval_hash, eval_white);
  if (eval_hit)
    {
    return (pos.side_to_move() == white) ? eval_white : -eval_white;
    }
  int score = eval_lazy(pos);
  if (score - lazy_eval_margin >= beta)
    {
    return score;
    }
  if (score + lazy_eval_margin > alpha)
    {
    score = eval(pos);
    eval_white = (pos.side_to_move() == white) ? score : -score;
    store_in_eval_table(evaltable, eval_hash, eval_white);
    }
  return score;
  }

void print_eval(std::ostream& str, const position& pos)
  {
  int v = eval(pos);

  str << "Total evaluation for WHITE = " << (pos.side_to_move() == white ? v : -v) << std::endl;
  str << "Total evaluation for BLACK = " << (pos.side_to_move() == white ? -v : v) << std::endl;  
  str << "Pieces score for WHITE = " << mg_value(pos.non_pawn_material_value(white)) << std::endl;
  str << "Pieces score for BLACK = " << mg_value(pos.non_pawn_material_value(black)) << std::endl;
  str << "Endgame pieces score for WHITE = " << eg_value(pos.non_pawn_material_value(white)) << std::endl;
  str << "Endgame pieces score for BLACK = " << eg_value(pos.non_pawn_material_value(black)) << std::endl;
  str << "Pawn score for WHITE = " << mg_value(pos.pawn_material_value(white)) << std::endl;
  str << "Pawn score for BLACK = " << mg_value(pos.pawn_material_value(black)) << std::endl;
  str << "Endgame pawn score for WHITE = " << eg_value(pos.pawn_material_value(white)) << std::endl;
  str << "Endgame pawn score for BLACK = " << eg_value(pos.pawn_material_value(black)) << std::endl;
  str << "Piece square score for WHITE = " << mg_value(pos.positional_value(white)) << std::endl;
  str << "Piece square score for BLACK = " << mg_value(pos.positional_value(black)) << std::endl;
  str << "Endgame piece square score for WHITE = " << eg_value(pos.positional_value(white)) << std::endl;
  str << "Endgame piece square score for BLACK = " << eg_value(pos.positional_value(black)) << std::endl;
  str << "Bonuses for WHITE = " << eval_data.WhiteBonus << std::endl;
  str << "Bonuses for BLACK = " << eval_data.BlackBonus << std::endl;
  str << "Tropism to BLACK king = " << eval_data.TropismToBlackKing << std::endl;
  str << "Tropism to WHITE king = " << eval_data.TropismToWhiteKing << std::endl;
  str << "Shield for WHITE king= " << eval_data.WhiteKingShield << std::endl;
  str << "Shield for BLACK king = " << eval_data.BlackKingShield << std::endl;
  str << "Safety for WHITE king= " << eval_data.WhiteKingSafety << std::endl;
  str << "Safety for BLACK king = " << eval_data.BlackKingSafety << std::endl;
  str << "Attack to BLACK king score = " << eval_data.AttackBlackKingScore << std::endl;
  str << "Attack to WHITE king score = " << eval_data.AttackWhiteKingScore << std::endl;
  str << "Mobility score for WHITE = " << eval_data.WhiteMobilityScore << std::endl;
  str << "Mobility score for BLACK = " << eval_data.BlackMobilityScore << std::endl;
  str << "Pawn pattern score for WHITE = " << eval_data.pawn_eval[0] << std::endl;
  str << "Pawn pattern score for BLACK = " << eval_data.pawn_eval[1] << std::endl;
  str << "Pattern score for WHITE = " << eval_data.trapped_opening[0] << std::endl;
  str << "Pattern score for BLACK = " << eval_data.trapped_opening[1] << std::endl;
  str << "Pattern score for WHITE endgame = " << eval_data.trapped_endgame[0]<< std::endl;
  str << "Pattern score for BLACK endgame = " << eval_data.trapped_endgame[1]<< std::endl;
  }