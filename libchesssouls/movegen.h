#pragma once

#include "types.h"
#include "libchesssouls_api.h"

class position;

enum e_movegentype
  {
  captures,  
  non_evasions,
  quiet,
  evasions,
  legal
  };

template <e_movegentype T>
LIB_CHESSSOULS_API move* generate(const position& pos, move* mlist);

template <>
LIB_CHESSSOULS_API move* generate<legal>(const position& pos, move* mlist);

template <e_movegentype T>
struct movelist
  {
  movelist(const position& pos) : current(mlist),
    last(generate<T>(pos, mlist))
    {
    *last = move_none;
    }

  void operator++()
    {
    ++current;
    }

  move operator[] (size_t index) const
    {
    return mlist[index];
    }

  move operator *() const
    {
    return *current;
    }

  size_t size() const
    {
    return last - mlist;
    }

  bool contains(move m) const
    {
    const move* it(mlist);
    for (; it != last; ++it)
      {
      if (*it == m)
        return true;
      }
    return false;
    }

  private:
    move mlist[max_moves];
    move* current;
    move* last;
  };

