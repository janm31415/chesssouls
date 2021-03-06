#include "movepick.h"
#include "position.h"
#include "search.h"

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
      _score[curr - _first] = ctxt.winning_capture_move_ordering_score + (pc << 4) - pos.piece_on(from);
    else
      _score[curr - _first] = ctxt.history[from][to];
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