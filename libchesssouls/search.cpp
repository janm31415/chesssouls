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

pv_line pv[max_ply];
int history[nr_squares][nr_squares];
int max_depth = 4;
int time_limit = 9999999999;
uint64_t node_limit = std::numeric_limits<uint64_t>::max();
bool stop_search;
bool follow_pv;
int ply; // the number of half moves since the start of the search
bool use_book = true;

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

  void checkup()
    {
    if (get_ms() >= time_limit || nodes > node_limit)
      {
      stop_search = true;
      }
    }

  int quiesce(position& pos, int alpha, int beta)
    {

    ++nodes;

    //if ((nodes & 1023) == 0)
    //  checkup();

    pv[ply].nr_of_moves = ply;

    if (ply >= max_ply - 1)
      return eval(pos);

    int score = eval(pos);
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
    move_picker mp(mlist, last, pos);

    bitboard pinned = pos.pinned_pieces(pos.side_to_move());

    while (!mp.done())
      {
      move current = mp.next();
      if (!pos.legal(current, pinned))
        continue;
      pos.do_move(current);
      ++ply;
      int score = -quiesce(pos, -beta, -alpha);
      pos.undo_move(current);
      --ply;
      if (score > alpha)
        {
        /* update the PV */
        pv[ply].moves[ply] = current;
        for (int j = ply + 1; j < pv[ply + 1].nr_of_moves; ++j)
          pv[ply].moves[j] = pv[ply + 1].moves[j];
        pv[ply].nr_of_moves = pv[ply + 1].nr_of_moves;

        if (score > beta)
          return beta;
        alpha = score;
        }
      }
    return alpha;
    }

  int negamax(position& pos, int alpha, int beta, int depth)
    {
    if (!depth)
      {
      return quiesce(pos, alpha, beta);
      }
    ++nodes;

    //if ((nodes & 1023) == 0)
    //  checkup();

    pv[ply].nr_of_moves = ply;

    /* if this isn't the root of the search tree (where we have
     to pick a move and can't simply return 0) then check to
     see if the position is a repeat. if so, we can assume that
     this line is a draw and return 0. */
    if (ply && pos.repetitions())
      return 0;

    if (ply >= max_ply - 1)
      return eval(pos);

    /* are we in check? if so, we want to search deeper */
    if (pos.checkers())
      ++depth;

    move mlist[max_moves];
    move* last = generate<legal>(pos, mlist);   
    *last = move_none;
    move_picker mp(mlist, last, pos);

    while (!mp.done())
      {
      move current = mp.next();
      pos.do_move(current);
      ++ply;
      int score = -negamax(pos, -beta, -alpha, depth - 1);
      pos.undo_move(current);
      --ply;
      if (score > alpha)
        {
        /* this move caused a cutoff, so increase the history
         value so it gets ordered high next time we can
         search it */
        history[from_square(current)][to_square(current)] += depth*depth;

        /* update the PV */
        pv[ply].moves[ply] = current;
        for (int j = ply + 1; j < pv[ply + 1].nr_of_moves; ++j)
          pv[ply].moves[j] = pv[ply + 1].moves[j];
        pv[ply].nr_of_moves = pv[ply + 1].nr_of_moves;

        if (score > beta)
          return beta;
        alpha = score;        
        }
      }
    if (mlist == last)
      {
      /* no legal moves? then we're in checkmate or stalemate */
      if (pos.checkers())
        return -value_mate + ply;
      else
        return 0;
      }
    /* fifty move draw rule */
    if (pos.fifty() > 99)
      return 0;

    return alpha;
    } //negamax
  } // namespace

void think(position& pos, int output)
  {
  if (use_book)
    {
    move bm = book_move(pos);
    if (bm != move_none)
      {
      pv[0].moves[0] = bm;
      pv[0].nr_of_moves = 1;
      return;
      }
    }

  memset(pv, 0, sizeof(pv));
  memset(history, 0, sizeof(history));

  if (output == 1)
    std::cout << "ply      nodes  score  pv\n";

  stop_search = false;
  nodes = 0;
  ply = 0;
  int alpha = -value_mate-max_ply;
  int beta = value_mate+max_ply;

  set_start_time();

  for (int d = 1; d <= max_depth; ++d)
    {
    checkup();
    if (stop_search && d > 1)
      break;
    follow_pv = true;
    int score = negamax(pos, alpha, beta, d);
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
      for (int j = 0; j < pv[0].nr_of_moves; ++j)
        std::cout << " " << move_to_uci(pv[0].moves[j]);
      std::cout << "\n";
      fflush(stdout);
      }
    }

  }