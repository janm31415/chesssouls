#pragma once

#include "types.h"

class position;

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