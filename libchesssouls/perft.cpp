#include "perft.h"
#include "position.h"
#include "movegen.h"


uint64_t perft(position& pos, int depth)
  {
  uint64_t nodes = 0;
  const bool leaf = (depth == 2);
  movelist<legal> it(pos);
  for (; *it; ++it)
    {
    if (depth <= 1)
      ++nodes;
    else
      {
      pos.do_move(*it);
      nodes += leaf ? movelist<legal>(pos).size() : perft(pos, depth - 1);
      pos.undo_move(*it);
      }
    }
  return nodes;
  }