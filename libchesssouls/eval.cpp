#include "eval.h"
#include "position.h"

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
    -50, -40, -30, -25, -25, -30, -40, -50
    - 35, -25, -15, -10, -10, -15, -25, -35,
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

void init_eval()
  {
  for (e_square s1 = sq_a1; s1 <= sq_h8; ++s1)
    {
    pcsq[white][pawn][s1] = make_score(pawn_pcsq_mg[s1], pawn_pcsq_eg[s1]);
    pcsq[white][knight][s1] = make_score(knight_pcsq_mg[s1], knight_pcsq_eg[s1]);
    pcsq[white][bishop][s1] = make_score(bishop_pcsq_mg[s1], bishop_pcsq_eg[s1]);
    pcsq[white][rook][s1] = make_score(rook_pcsq_mg[s1], rook_pcsq_eg[s1]);
    pcsq[white][queen][s1] = make_score(queen_pcsq_mg[s1], queen_pcsq_eg[s1]);
    pcsq[white][king][s1] = make_score(king_pcsq_mg[s1], king_pcsq_eg[s1]);
    pcsq[black][pawn][~s1] = make_score(pawn_pcsq_mg[s1], pawn_pcsq_eg[s1]);
    pcsq[black][knight][~s1] = make_score(knight_pcsq_mg[s1], knight_pcsq_eg[s1]);
    pcsq[black][bishop][~s1] = make_score(bishop_pcsq_mg[s1], bishop_pcsq_eg[s1]);
    pcsq[black][rook][~s1] = make_score(rook_pcsq_mg[s1], rook_pcsq_eg[s1]);
    pcsq[black][queen][~s1] = make_score(queen_pcsq_mg[s1], queen_pcsq_eg[s1]);
    pcsq[black][king][~s1] = make_score(king_pcsq_mg[s1], king_pcsq_eg[s1]);
    }
  piece_value[pawn] = make_score(pawn_value_mg, pawn_value_eg);
  piece_value[knight] = make_score(knight_value_mg, knight_value_eg);
  piece_value[bishop] = make_score(bishop_value_mg, bishop_value_eg);
  piece_value[rook] = make_score(rook_value_mg, rook_value_eg);
  piece_value[queen] = make_score(queen_value_mg, queen_value_eg);
  piece_value[king] = 0;

  total_piece_score_opening = (piece_value[knight] * 2 + piece_value[bishop] * 2 + piece_value[rook] * 2 + piece_value[queen]) >> 16;
  }

int eval(const position& pos)
  {
  int mat_white = pos.non_pawn_material_value(white);
  int mat_black = pos.non_pawn_material_value(black);
  int pawn_white = pos.pawn_material_value(white);
  int pawn_black = pos.pawn_material_value(black);
  int pos_white = pos.positional_value(white);
  int pos_black = pos.positional_value(black);

  int mat_white_mg = mat_white >> 16;
  int mat_black_mg = mat_black >> 16;

  int score_white = mat_white + pawn_white + pos_white;
  int score_black = mat_black + pawn_black + pos_black;

  int score_white_mg = score_white >> 16;
  int score_black_mg = score_black >> 16;

  int score_white_eg = score_white & 0xffff;
  int score_black_eg = score_black & 0xffff;

  int ph = mat_white_mg + mat_black_mg;
  ph = (ph * 256 + total_piece_score_opening) / (2 * total_piece_score_opening);
  
  int sc = (ph*(score_white_mg - score_black_mg) + (256 - ph)*(score_white_eg - score_black_eg)) / 256;

  return pos.side_to_move() == white ? sc : -sc;  
  }