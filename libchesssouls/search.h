#pragma once
#include "libchesssouls_api.h"
#include "types.h"

class position;

struct search_context
  {
  LIB_CHESSSOULS_API search_context();
  int ply;
  bool follow_pv;
  int max_depth;
  int time_limit;
  uint64_t node_limit;
  bool stop_search;
  bool use_book;
  pv_line pv[max_ply]; // principal variation
  pv_line main_pv;
  int move_step; // normally equal to 1, but if you want to handicap the engine, you can skip moves.
  int history[nr_squares][nr_squares];
  bool fail_soft;
  bool use_transposition;
  uint16_t hash_move[2];
  int hash_move_ordering_score;
  int pv_move_ordering_score;
  int winning_capture_move_ordering_score;
  int aspiration_window_size;
  int aspiration_window_correction;
  bool null_move_pruning;
  int null_move_reduction;
  };

LIB_CHESSSOULS_API void think(position& pos, int output, search_context& ctxt);