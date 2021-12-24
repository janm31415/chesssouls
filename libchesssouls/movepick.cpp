#include "movepick.h"
#include "position.h"
#include "search.h"
#include "eval.h"

move_picker::move_picker(move* first, move* last, const position& pos, search_context& ctxt) :
  _first(first), _last(last), _pos(pos), _current(first), _ctxt(ctxt)
  {
  bool check_pv = _ctxt.follow_pv;
  _ctxt.follow_pv = false;

  for (move* curr = _first; curr != _last; ++curr)
    {
    e_square from = from_square(*curr);
    e_square to = to_square(*curr);
    e_piece pc = pos.piece_on(to);
    if (pc != no_piece)
      {
      int see = pos.see(*curr, 0);
      if (see > 0)
        _score[curr - _first] = ctxt.winning_capture_move_ordering_score + see;
      else if (see == 0)
        _score[curr - _first] = ctxt.equal_capture_move_ordering_score + pos.move_ordering_score(*curr);//(pc << 4) - pos.piece_on(from);
      else
        _score[curr - _first] = ctxt.losing_capture_move_ordering_score + see;//(pc << 4) - pos.piece_on(from);
      }
    else if (type_of(*curr) == enpassant)
      {
      _score[curr - _first] = ctxt.equal_capture_move_ordering_score + piece_value_see[pawn];
      }
    else if (type_of(*curr) == castling)
      {
      _score[curr - _first] = ctxt.castle_move_score;
      }
    else if (type_of(*curr) == promotion)
      {
      _score[curr - _first] = ctxt.promotion_move_ordering_score;
      }
    else
      {
      _score[curr - _first] = ctxt.history_move_ordering_score + ctxt.history[from][to];// + pos.move_ordering_score(*curr);
      // killer moves are quiet moves
#ifdef KILLER_MOVES
      for (int j = 0; j < ctxt.max_killers; ++j)
        {
        if (*curr == ctxt.killer_moves[ctxt.ply][j])
          {
          _score[curr - _first] += ctxt.killer_move_ordering_score;
          }
        }
      if (ctxt.use_mate_killer)
        {
        for (int j = 0; j < ctxt.max_mate_killers; ++j)
          {
          if (*curr == ctxt.killer_mate_moves[ctxt.ply][j])
            {
            _score[curr - _first] += ctxt.killer_mate_move_ordering_score;
            }
          }
        }
#endif
      }
    if (check_pv && *curr == _ctxt.main_pv.moves[_ctxt.ply])
      {
      _ctxt.follow_pv = true;
      _score[curr - _first] += ctxt.pv_move_ordering_score;
      }
    if (*curr == ctxt.hash_move[0] || *curr == ctxt.hash_move[1])
      {
      _score[curr - _first] += ctxt.hash_move_ordering_score;
      }
    }

  }

move_picker::~move_picker()
  {

  }

bool move_picker::done() const
  {
  return _current >= _last;
  }

move move_picker::next()
  {
  move* best_move = _current;
  int sc = _score[best_move - _first];
  for (move* it = _current + 1; it != _last; ++it)
    {
    if (_score[it - _first] > sc)
      {
      best_move = it;
      sc = _score[best_move - _first];
      }
    }
  move tmp = *_current;
  *_current = *best_move;
  *best_move = tmp;
  _score[best_move - _first] = _score[_current - _first];
  auto ret = *_current;
  _current += _ctxt.move_step;
  return ret;
  }