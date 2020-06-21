#include "search.h"
#include "book.h"
#include "position.h"
#include "movegen.h"
#include "notation.h"
#include "eval.h"
#include "movepick.h"

#include <iostream>
#include <iomanip>
#include <chrono>
#include <limits>

search_context::search_context()
  {
  max_depth = 4;
  time_limit = std::numeric_limits<int>::max();
  node_limit = std::numeric_limits<uint64_t>::max();
  stop_search = false;
  follow_pv = true;
  ply = 0;
  use_book = true;  
  }

namespace
  {
  
  int nodes;
  

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

  int quiesce(position& pos, int alpha, int beta, search_context& ctxt)
    {    

    ++nodes;

    if ((nodes & 1023) == 0)
      checkup(ctxt);

    ctxt.pv[ctxt.ply].nr_of_moves = ctxt.ply;

    if (ctxt.ply >= max_ply - 1)
      return eval(pos);

    int score = eval(pos);

    if (ctxt.stop_search)
      return score;

    if (score >= beta)
      return beta;
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
      pos.do_move(current);
      ++ctxt.ply;
      score = -quiesce(pos, -beta, -alpha, ctxt);
      pos.undo_move(current);
      --ctxt.ply;
      if (score > alpha)
        {
        /* update the PV */
        ctxt.pv[ctxt.ply].moves[ctxt.ply] = current;
        for (int j = ctxt.ply + 1; j < ctxt.pv[ctxt.ply + 1].nr_of_moves; ++j)
          ctxt.pv[ctxt.ply].moves[j] = ctxt.pv[ctxt.ply + 1].moves[j];
        ctxt.pv[ctxt.ply].nr_of_moves = ctxt.pv[ctxt.ply + 1].nr_of_moves;

        if (score >= beta)
          return beta;
        alpha = score;
        }
      }
    return alpha;
    }

  int negamax(position& pos, int alpha, int beta, int depth, search_context& ctxt)
    {
    if (!depth)
      {
      return quiesce(pos, alpha, beta, ctxt);
      }
    ++nodes;

    //if ((nodes & 1023) == 0)
    //  checkup();

    ctxt.pv[ctxt.ply].nr_of_moves = ctxt.ply;

    /* if this isn't the root of the search tree (where we have
     to pick a move and can't simply return 0) then check to
     see if the position is a repeat. if so, we can assume that
     this line is a draw and return 0. */
    if (ctxt.ply && pos.repetitions())
      return 0;

    if (ctxt.ply >= max_ply - 1)
      return eval(pos);

    /* are we in check? if so, we want to search deeper */
    if (pos.checkers())
      ++depth;

    move mlist[max_moves];
    move* last = generate<legal>(pos, mlist);   
    *last = move_none;
    move_picker mp(mlist, last, pos, ctxt);

    while (!mp.done())
      {
      move current = mp.next();
      pos.do_move(current);
      ++ctxt.ply;
      int score = -negamax(pos, -beta, -alpha, depth - 1, ctxt);
      pos.undo_move(current);
      --ctxt.ply;
      if (score > alpha)
        {
        /* this move caused a cutoff, so increase the history
         value so it gets ordered high next time we can
         search it */
        ctxt.history[from_square(current)][to_square(current)] += depth*depth;

        /* update the PV */
        ctxt.pv[ctxt.ply].moves[ctxt.ply] = current;
        for (int j = ctxt.ply + 1; j < ctxt.pv[ctxt.ply + 1].nr_of_moves; ++j)
          ctxt.pv[ctxt.ply].moves[j] = ctxt.pv[ctxt.ply + 1].moves[j];
        ctxt.pv[ctxt.ply].nr_of_moves = ctxt.pv[ctxt.ply + 1].nr_of_moves;

        if (score >= beta)
          return beta;
        alpha = score;        
        }
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

    return alpha;
    } //negamax
  } // namespace

void think(position& pos, int output, search_context& ctxt)
  {
  if (ctxt.use_book)
    {
    move bm = book_move(pos);
    if (bm != move_none)
      {
      ctxt.pv[0].moves[0] = bm;
      ctxt.pv[0].nr_of_moves = 1;
      return;
      }
    }

  memset(ctxt.pv, 0, sizeof(ctxt.pv));
  memset(ctxt.history, 0, sizeof(ctxt.history));

  if (output == 1)
    std::cout << "ply      nodes  score  pv\n";

  ctxt.stop_search = false;
  nodes = 0;
  ctxt.ply = 0;
  int alpha = -value_mate-max_ply;
  int beta = value_mate+max_ply;

  set_start_time();

  for (int d = 1; d <= ctxt.max_depth; ++d)
    {
    checkup(ctxt);
    if (ctxt.stop_search && d > 1)
      break;
    ctxt.follow_pv = true;
    int score = negamax(pos, alpha, beta, d, ctxt);
    assert(ply == 0);
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
      for (int j = 0; j < ctxt.pv[0].nr_of_moves; ++j)
        std::cout << " " << move_to_uci(ctxt.pv[0].moves[j]);
      std::cout << "\n";
      fflush(stdout);
      }
    }

  }