#include "test_movegen.h"
#include <libchesssouls/movegen.h>
#include <libchesssouls/position.h>
#include <string>
#include "test_assert.h"

namespace
  {

  void test_generate_moves_starting_pos()
    {
    std::string fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    position p(fen);
    movelist<legal> moves(p);

    TEST_EQ(20, moves.size());
    }

  }


void run_all_movegen_tests()
  {
  test_generate_moves_starting_pos();
  }