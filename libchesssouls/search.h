#pragma once
#include "libchesssouls_api.h"
#include "types.h"

class position;

struct search_context
  {
  LIB_CHESSSOULS_API search_context();
  LIB_CHESSSOULS_API void clear();
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
  int equal_capture_move_ordering_score;
  int losing_capture_move_ordering_score;
  int castle_move_score;
  int history_move_ordering_score;
  int promotion_move_ordering_score;
  bool use_aspiration_window;
  int aspiration_window_size;
  int aspiration_window_correction;
  bool null_move_pruning;
  int null_move_reduction;
  bool internal_iterative_deepening;
  bool late_move_reduction;
  int delta_pruning_margin;
  bool delta_pruning;  
  int max_history_ply;
  bool history_pruning;
  int history_threshold;
  int killer_mate_move_ordering_score;
  int killer_move_ordering_score;
  bool use_mate_killer;
  int max_killers;
  int max_mate_killers;
  move killer_moves[max_ply][5];
  move killer_mate_moves[max_ply][5];
  };

LIB_CHESSSOULS_API void think(position& pos, int output, search_context& ctxt);