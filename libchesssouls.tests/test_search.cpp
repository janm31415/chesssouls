#include "test_search.h"
#include "test_assert.h"

#include <libchesssouls/movegen.h>
#include <libchesssouls/perft.h>
#include <libchesssouls/position.h>
#include <libchesssouls/search.h>

#include <string>

namespace
  {
  void test_bug_1()
    {
    max_depth = 4;
    time_limit = 999999999;
    std::string fen = "r1b1k1nr/p1b4p/4p1p1/3p1p2/K3p3/8/P1P2q1P/8 b kq - 1 22";
    position pos(fen);
    think(pos, 0);
    TEST_EQ(5, pv[0].nr_of_moves);
    TEST_EQ(842, pv[0].moves[0]);
    }

  void test_bug_2()
    {
    max_depth = 4;
    time_limit = 999999999;
    node_limit = 2048;
    std::string fen = "B7/4Q3/7k/6B1/3P3N/P7/P1P2KPP/R4R2 b - -4 27";
    position pos(fen);
    think(pos, 1);
    TEST_EQ(2, pv[0].nr_of_moves);
    TEST_EQ(3047, pv[0].moves[0]);
    }

  void test_bug_3()
    {
    max_depth = 4;
    time_limit = 999999999;
    node_limit = 2048;
    std::string fen = "r1bqkbnr/p4pp1/1pn1p2p/3p4/3P4/2NB1N2/PPP2PPP/R1BQ1RK1 w kq - 0 8";
    position pos(fen);
    move m = make_move(sq_d3, sq_f5);
    pos.do_move(m);
    think(pos, 1);
    TEST_EQ(5, pv[0].nr_of_moves);
    TEST_EQ(2853, pv[0].moves[0]);
    }
  }

void run_all_search_tests()
  {
  test_bug_1();
  test_bug_2();
  test_bug_3();
  }