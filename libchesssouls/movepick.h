#pragma once

#include "types.h"
#include "libchesssouls_api.h"

class position;

LIB_CHESSSOULS_API extern int move_step; // normally equal to 1, but if you want to handicap the engine, you can skip moves.

class move_picker
  {
  public:
    move_picker(move* first, move* last, const position& pos);
    ~move_picker();

    move next();

    bool done() const;

  private:
    move* _first;
    move* _current;
    move* _last;
    const position& _pos;
    int _score[max_moves];
  };