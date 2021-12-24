#include "search.h"
#include "book.h"
#include "position.h"
#include "movegen.h"
#include "notation.h"
#include "eval.h"
#include "movepick.h"
#include "trans.h"

#include <iostream>
#include <iomanip>
#include <chrono>
#include <limits>
#include <algorithm>

void clear_killer_moves(search_context& ctxt)
  {
  memset(ctxt.killer_moves, 0, sizeof(ctxt.killer_moves));
  memset(ctxt.killer_mate_moves, 0, sizeof(ctxt.killer_mate_moves));
  }

search_context::search_context()
  {
  max_depth = 10;
  time_limit = std::numeric_limits<int>::max();
  node_limit = std::numeric_limits<uint64_t>::max();
  stop_search = false;
  follow_pv = true;
  ply = 0;
  use_book = true;  
  move_step = 1;
  fail_soft = true;
  use_transposition = true;
  hash_move[0] = 0;
  hash_move[1] = 0;
  pv_move_ordering_score = 1000000;
  hash_move_ordering_score = 900000;
  winning_capture_move_ordering_score = 800000;
  promotion_move_ordering_score = 700000;
  killer_mate_move_ordering_score = 950000;
  killer_move_ordering_score = 600000;
  equal_capture_move_ordering_score = 500000;
  castle_move_score = 400000;
  losing_capture_move_ordering_score = 200000;
  history_move_ordering_score = 100000;
  aspiration_window_size = 100;
  aspiration_window_correction = 25;
  null_move_pruning = true;
  null_move_reduction = 2;
  use_aspiration_window = true;
  internal_iterative_deepening = true;
  late_move_reduction = true;
  delta_pruning_margin = 200;
  delta_pruning = true;
  max_history_ply = 10;
  history_pruning = false;
  history_threshold = 9830;

  futility_pruning = true;
  futility_skip = false;
  futility_reduction = 1;
  futility_margin_depth_1 = 200;
  futility_margin_depth_2 = 500;

  use_mate_killer = false;
  max_killers = 2;
  max_mate_killers = 1; 

  clear_killer_moves(*this);
  }

void search_context::clear()
  {
  clear_killer_moves(*this);
  }

uint64_t nodes;

namespace
  {  

  std::chrono::time_point<std::chrono::system_clock> start_time;

  void set_start_time()
    {
    start_time = std::chrono::system_clock::now();
    }

  int get_ms()
    {
    std::chrono::time_point<std::chrono::system_clock> current_time = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = current_time - start_time;
    return int(elapsed.count()*1000.0);
    }

  void checkup(search_context& ctxt)
    {
    if (get_ms() >= ctxt.time_limit || nodes > ctxt.node_limit)
      {
      ctxt.stop_search = true;
      }
    }


  void record_hash(const position& pos, int depth, int score, int node_type, const move& b)
    {
    store_in_transposition_table(transposition, pos.zobrist_key(), b, depth, score, node_type);
    }

  bool probe_hash(const position& pos, const search_context& ctxt, transposition_table& trans, uint16_t hash_moves[2], int& score, int depth, int alpha, int beta)
    {
    int hash_depth1, hash_depth2;
    int node_type1, node_type2;
    int score1, score2;
    if (find_in_transposition_table(trans, pos.zobrist_key(), hash_moves[0], hash_depth1, score1, node_type1, hash_moves[1], hash_depth2, score2, node_type2))
      {
      assert(node_type1 >= 0 && node_type1 <= 2);
      assert(node_type2 >= 0 && node_type2 <= 2);
      assert(hash_depth1 >= 0);
      assert(hash_depth2 >= 0);
      if (hash_depth1 >= depth)
        {
        if (node_type1 == ALPHA_NODE && score1 <= alpha)
          {
          score = ctxt.fail_soft ? score1 : alpha;
          return true;
          }
        if (node_type1 == BETA_NODE && score1 >= beta)
          {
          score = ctxt.fail_soft ? score1 : beta;
          return true;
          }
        }
      if (hash_depth2 >= depth)
        {
        if (node_type2 == ALPHA_NODE && score2 <= alpha)
          {
          score = ctxt.fail_soft ? score2 : alpha;
          return true;
          }
        if (node_type2 == BETA_NODE && score2 >= beta)
          {
          score = ctxt.fail_soft ? score2 : beta;
          return true;
          }
        }
      return false;
      }
    return false;
    }

  int quiesce(position& pos, int alpha, int beta, search_context& ctxt)
    {    
    ++nodes;

    if ((nodes & 1023) == 0)
      checkup(ctxt);

    ctxt.pv[ctxt.ply].nr_of_moves = ctxt.ply;

    if (ctxt.ply >= max_ply - 1)
      return eval(pos, alpha, beta);

    int score = eval(pos, alpha, beta);
    int bestscore = score;
    int material = score;

    if (ctxt.stop_search)
      return 0;

    if (score >= beta)
      return ctxt.fail_soft ? score : beta;
    if (score > alpha)
      alpha = score;

    move mlist[max_moves];
    move* last;
    if (pos.checkers())
      last = generate<legal>(pos, mlist);
    else
      last = generate<captures>(pos, mlist);
    *last = move_none;
    move_picker mp(mlist, last, pos, ctxt);

    bitboard pinned = pos.pinned_pieces(pos.side_to_move());

    while (!mp.done())
      {
      move current = mp.next();
      if (!pos.legal(current, pinned))
        continue;

      if (ctxt.delta_pruning && !pos.checkers() && !pos.endgame())
        {
        int cap = material;
        if (type_of(current) == enpassant)
          {
          cap += piece_value_see[pawn];
          }
        else if (type_of(current) == promotion)
          {
          cap += piece_value_see[type_of(pos.piece_on(to_square(current)))] - piece_value_see[pawn];
          }
        else
          {
          cap += pos.see(current, 0);
          }
        if (cap < (alpha - ctxt.delta_pruning_margin))
          continue;
        }      
      pos.do_move(current);
      ++ctxt.ply;
      score = -quiesce(pos, -beta, -alpha, ctxt);
      pos.undo_move(current);
      --ctxt.ply;
      if (ctxt.stop_search)
        return 0;
      if (ctxt.fail_soft)
        {
        if (score > bestscore)
          bestscore = score;
        }
#if defined(BETA_FIRST)
      if (score >= beta)
        return ctxt.fail_soft ? score : beta;
#endif
      if (score > alpha)
        {
        /* update the PV */
        ctxt.pv[ctxt.ply].moves[ctxt.ply] = current;
        for (int j = ctxt.ply + 1; j < ctxt.pv[ctxt.ply + 1].nr_of_moves; ++j)
          ctxt.pv[ctxt.ply].moves[j] = ctxt.pv[ctxt.ply + 1].moves[j];
        ctxt.pv[ctxt.ply].nr_of_moves = ctxt.pv[ctxt.ply + 1].nr_of_moves;
#if !defined(BETA_FIRST)
        if (score >= beta)
          return ctxt.fail_soft ? score : beta;
#endif

        alpha = score;
        }
      }
    return ctxt.fail_soft ? bestscore : alpha;
    }

  int negamax(position& pos, int alpha, int beta, int depth, bool allow_null_move_pruning, int expected_node_type, search_context& ctxt)
    {
    if (depth<=0)
      {
      return quiesce(pos, alpha, beta, ctxt);
      }
    ++nodes;

    if ((nodes & 1023) == 0)
      checkup(ctxt);

    int bestscore = -value_mate;
    int score = -value_mate;
    move best_move = move_none;

    ctxt.pv[ctxt.ply].nr_of_moves = ctxt.ply;

    /* if this isn't the root of the search tree (where we have
     to pick a move and can't simply return 0) then check to
     see if the position is a repeat. if so, we can assume that
     this line is a draw and return 0. */
    if (ctxt.ply && pos.repetitions())
      return 0;

    ctxt.hash_move[0] = ctxt.hash_move[1] = 0;

    if (ctxt.use_transposition)
      {
      if (ctxt.ply > 1)
        {
        if (probe_hash(pos, ctxt, transposition, ctxt.hash_move, score, depth, alpha, beta))
          return score;
        }
      }

    if (ctxt.ply >= max_ply - 1)
      return eval(pos, alpha, beta);

    int node_type = ALPHA_NODE;

    /* are we in check? if so, we want to search deeper */
    if (pos.checkers())
      ++depth;
    else // not in check
      {
      if (allow_null_move_pruning && !ctxt.follow_pv && expected_node_type != PV_NODE && depth > ctxt.null_move_reduction)
        {
        pos.do_null_move();
        score = -negamax(pos, -beta, -beta + 1, depth - ctxt.null_move_reduction - 1, false, OPPOSITE(expected_node_type), ctxt);
        pos.undo_null_move();
        if (score >= beta)
          {
          return ctxt.fail_soft ? score : beta;
          }
        }        
      }

    // internal iterative deepening
    if (ctxt.use_transposition && ctxt.internal_iterative_deepening)
      {
      if (ctxt.hash_move[0] == 0 && expected_node_type == PV_NODE && depth > 6 && ctxt.ply > 1 && allow_null_move_pruning)
        {
        int new_depth = depth - 2;
        auto temp_score = negamax(pos, alpha, beta, new_depth, allow_null_move_pruning, expected_node_type, ctxt);
        if (temp_score > alpha)
          {
          if (temp_score < beta)
            {
            probe_hash(pos, ctxt, transposition, ctxt.hash_move, temp_score, depth, alpha, beta);            
            }
          }
        }
      }

#ifdef FUTILITY_PRUNING
    bool futility_prune = false;
    int futility_material = 0;
    int futility_margin = 0;
    if (ctxt.futility_pruning && depth <= 2 && !pos.checkers() && expected_node_type != PV_NODE && alpha < 9000 && beta < 9000 && alpha > -9000 && beta > -9000)
      {
      futility_prune = true;
      futility_margin = depth == 1 ? ctxt.futility_margin_depth_1 : ctxt.futility_margin_depth_2;
      }
#endif

    move mlist[max_moves];
    move* last = generate<legal>(pos, mlist);   
    *last = move_none;
    move_picker mp(mlist, last, pos, ctxt);

    int move_count = 0;

    while (!mp.done())
      {
      move current = mp.next();

      int reduction = 0;
#ifdef FUTILITY_PRUNING
      if (futility_prune && !pos.checkers() && !pos.capture_or_promotion(current)) // futility pruning
        {
        futility_material = eval_lazy(pos);
        if ((futility_material + futility_margin) <= alpha)
          {
          if (ctxt.fail_soft)
            {
            if ((futility_material + futility_margin) > bestscore)
              bestscore = (futility_material + futility_margin);
            }
          if (ctxt.futility_skip)
            {
            continue;
            }
          reduction += ctxt.futility_reduction;
          }
        }
#endif
      ++move_count;
      if (ctxt.late_move_reduction && move_count > 4 && depth >= 3 && !pos.checkers() && !ctxt.follow_pv && node_type != PV_NODE && ctxt.ply >= 2 && !pos.capture_or_promotion(current))
        {
        if (move_count <= 10)
          ++reduction;
        else
          reduction += 2;
        }
      // history pruning
#ifdef HISTORY_PRUNING
      if (ctxt.history_pruning && depth >= 3 && node_type != PV_NODE && !pos.checkers() && ctxt.ply >= 2)
        {
        if (ctxt.history[from_square(current)][to_square(current)] < ctxt.history_threshold)
          {
          if (reduction < 2)
            ++reduction;
          }
        }
#endif     
      
      pos.do_move(current);

      ++ctxt.ply;
      if ((expected_node_type != PV_NODE && node_type != PV_NODE) || move_count == 1)
        score = -negamax(pos, -beta, -alpha, depth - reduction - 1, ctxt.null_move_pruning, OPPOSITE(expected_node_type), ctxt);
      else
        {
        score = -negamax(pos, -alpha-1, -alpha, depth - reduction - 1, ctxt.null_move_pruning, BETA_NODE, ctxt);  // scout search are cut nodes
        if (score > alpha)
          score = -negamax(pos, -beta, -alpha, depth - reduction - 1, ctxt.null_move_pruning, PV_NODE, ctxt); // Children of PV-nodes that have to be re-searched because the scout search failed high, are PV-nodes
        }
      if (reduction && score >= beta) // search again if we reduced and get a cut
        {
        score = -negamax(pos, -beta, -alpha, depth - 1, ctxt.null_move_pruning, OPPOSITE(expected_node_type), ctxt);
        }
      pos.undo_move(current);
      --ctxt.ply;
      if (ctxt.stop_search)
        return 0;
      if (score > bestscore)
        {
        bestscore = score;
        }

#if defined(BETA_FIRST)
      if (score >= beta)
        {
        if (ctxt.ply <= ctxt.max_history_ply && !pos.capture(current))
          ctxt.history[from_square(current)][to_square(current)] += depth * depth;
        if (ctxt.use_transposition)
          record_hash(pos, depth, beta, BETA_NODE, current);
#ifdef KILLER_MOVES
        if (!pos.capture_or_promotion(current)) // killer heuristic
          {
          if (ctxt.use_mate_killer)
            {
            if (score >= 9000)
              {
              for (int j = ctxt.max_mate_killers - 2; j >= 0; --j)
                ctxt.killer_mate_moves[ctxt.ply][j + 1] = ctxt.killer_mate_moves[ctxt.ply][j];
              ctxt.killer_mate_moves[ctxt.ply][0] = current;
              }
            else
              {
              for (int j = ctxt.max_killers - 2; j >= 0; --j)
                ctxt.killer_moves[ctxt.ply][j + 1] = ctxt.killer_moves[ctxt.ply][j];
              ctxt.killer_moves[ctxt.ply][0] = current;
              }
            }
          else
            {
            for (int j = ctxt.max_killers - 2; j >= 0; --j)
              ctxt.killer_moves[ctxt.ply][j + 1] = ctxt.killer_moves[ctxt.ply][j];
            ctxt.killer_moves[ctxt.ply][0] = current;
            }
          }
#endif
        return ctxt.fail_soft ? score : beta;
        }
#endif

      if (score > alpha)
        {
        best_move = current;
        node_type = PV_NODE;
        /* this move caused a cutoff, so increase the history
         value so it gets ordered high next time we can
         search it */
        if (ctxt.ply <= ctxt.max_history_ply)
          {
          ctxt.history[from_square(current)][to_square(current)] += depth*depth;
          //if (ctxt.history[from_square(current)][to_square(current)] > 100000)
          //  {
          //  for (int i = 0; i < 64; ++i)
          //    for (int j = 0; j < 64; ++j)
          //      ctxt.history[i][j] /= 2;
          //  }
          }

        /* update the PV */
        ctxt.pv[ctxt.ply].moves[ctxt.ply] = current;
        for (int j = ctxt.ply + 1; j < ctxt.pv[ctxt.ply + 1].nr_of_moves; ++j)
          ctxt.pv[ctxt.ply].moves[j] = ctxt.pv[ctxt.ply + 1].moves[j];
        ctxt.pv[ctxt.ply].nr_of_moves = ctxt.pv[ctxt.ply + 1].nr_of_moves;
#if !defined(BETA_FIRST)
        if (score >= beta)
          {          
          if (ctxt.use_transposition)
            record_hash(pos, depth, beta, BETA_NODE, current);
#ifdef KILLER_MOVES
          if (!pos.capture_or_promotion(current)) // killer heuristic
            {
            if (ctxt.use_mate_killer)
              {
              if (score >= 9000)
                {
                for (int j = ctxt.max_mate_killers - 2; j >= 0; --j)
                  ctxt.killer_mate_moves[ctxt.ply][j + 1] = ctxt.killer_mate_moves[ctxt.ply][j];
                ctxt.killer_mate_moves[ctxt.ply][0] = current;
                }
              else
                {
                for (int j = ctxt.max_killers - 2; j >= 0; --j)
                  ctxt.killer_moves[ctxt.ply][j + 1] = ctxt.killer_moves[ctxt.ply][j];
                ctxt.killer_moves[ctxt.ply][0] = current;
                }
              }
            else
              {
              for (int j = ctxt.max_killers - 2; j >= 0; --j)
                ctxt.killer_moves[ctxt.ply][j + 1] = ctxt.killer_moves[ctxt.ply][j];
              ctxt.killer_moves[ctxt.ply][0] = current;
              }
            }
#endif
          return ctxt.fail_soft ? score : beta;
          }
#endif
        alpha = score;        
        }

      if (expected_node_type == BETA_NODE) // A Cut-node becomes an All-node once the first and all candidate cutoff moves are searched
        expected_node_type = ALPHA_NODE;
      }
    if (mlist == last)
      {
      /* no legal moves? then we're in checkmate or stalemate */
      if (pos.checkers())
        return -value_mate + ctxt.ply;
      else
        return 0;
      }
    /* fifty move draw rule */
    if (pos.fifty() > 99)
      return 0;

    if (ctxt.use_transposition)
      record_hash(pos, depth, alpha, node_type, best_move);

    return ctxt.fail_soft ? bestscore : alpha;
    } //negamax
  } // namespace

void think(position& pos, int output, search_context& ctxt)
  {
  if (ctxt.use_book)
    {
    move bm = book_move(pos);
    if (bm != move_none)
      {
      ctxt.main_pv.moves[0] = bm;
      ctxt.main_pv.nr_of_moves = 1;
      return;
      }
    }

  memset(ctxt.pv, 0, sizeof(ctxt.pv));
  memset(ctxt.history, 0, sizeof(ctxt.history));
  ctxt.main_pv.nr_of_moves = 0;

  if (output == 1)
    std::cout << "ply      nodes  score  pv\n";    

  ctxt.stop_search = false;
  nodes = 0;
  ctxt.ply = 0;
  int alpha = -value_mate-max_ply;
  int beta = value_mate+max_ply;
  int aspiration_fail_alpha = 0;
  int aspiration_fail_beta = 0;

  set_start_time();

  for (int d = 1; d <= ctxt.max_depth; )
    {
    checkup(ctxt);
    if (ctxt.stop_search && d > 1)
      break;
    ctxt.follow_pv = true;
    int score = negamax(pos, alpha, beta, d, ctxt.null_move_pruning, PV_NODE, ctxt);
    if (ctxt.pv[0].nr_of_moves > 0 && aspiration_fail_alpha == 0 && aspiration_fail_beta == 0)
      {
      ctxt.main_pv.nr_of_moves = ctxt.pv[0].nr_of_moves;
      for (int j = 0; j < ctxt.main_pv.nr_of_moves; ++j)
        ctxt.main_pv.moves[j] = ctxt.pv[0].moves[j];
      }
    assert(ctxt.ply == 0);
    if (ctxt.stop_search)
      return;
    if (output == 1)
      {
      std::cout << std::setw(3) << d << "  " << std::setw(9) << nodes << "  " << std::setw(5) << score << " ";
      }
    else if (output == 2)
      {
      std::cout << d << " " << score << " " << get_ms() / 10 << " " << nodes;
      }
    if (output)
      {
      for (int j = 0; j < ctxt.main_pv.nr_of_moves; ++j)
        std::cout << " " << move_to_uci(ctxt.main_pv.moves[j]);
      std::cout << "\n";
      fflush(stdout);
      }
    if (ctxt.use_aspiration_window)
      {
      if (score <= alpha)
        {
        ++aspiration_fail_alpha;
        alpha = score - ctxt.aspiration_window_correction;
        if (aspiration_fail_alpha > 2)
          alpha = std::min((int)-value_mate, alpha);
        continue;
        }
      if (score >= beta)
        {
        ++aspiration_fail_beta;
        beta = score + ctxt.aspiration_window_correction;
        if (aspiration_fail_beta > 2)
          beta = std::max((int)value_mate, beta);
        continue;
        }
      alpha = score - ctxt.aspiration_window_size;
      beta = score + ctxt.aspiration_window_size;      
      aspiration_fail_alpha = 0;
      aspiration_fail_beta = 0;
      }
    ++d;
    }

  }