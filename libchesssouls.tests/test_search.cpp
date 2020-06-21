#include "test_search.h"
#include "test_assert.h"

#include <libchesssouls/movegen.h>
#include <libchesssouls/movepick.h>
#include <libchesssouls/perft.h>
#include <libchesssouls/position.h>
#include <libchesssouls/search.h>

#include <string>

namespace
  {
  void test_bug_1()
    {
    search_context ctxt;
    ctxt.max_depth = 4;
    ctxt.time_limit = 999999999;
    ctxt.move_step = 1;
    std::string fen = "r1b1k1nr/p1b4p/4p1p1/3p1p2/K3p3/8/P1P2q1P/8 b kq - 1 22";
    position pos(fen);
    think(pos, 0, ctxt);
    TEST_EQ(5, ctxt.pv[0].nr_of_moves);
    TEST_EQ(842, ctxt.pv[0].moves[0]);
    }

  void test_bug_2()
    {
    search_context ctxt;
    ctxt.max_depth = 4;
    ctxt.time_limit = 999999999;
    ctxt.move_step = 1;
    ctxt.node_limit = 2048;
    std::string fen = "B7/4Q3/7k/6B1/3P3N/P7/P1P2KPP/R4R2 b - -4 27";
    position pos(fen);
    think(pos, 0, ctxt);
    TEST_ASSERT(ctxt.pv[0].nr_of_moves > 0);
    TEST_EQ(3047, ctxt.pv[0].moves[0]);
    }

  void test_bug_3()
    {
    search_context ctxt;
    ctxt.max_depth = 4;
    ctxt.time_limit = 999999999;
    ctxt.move_step = 1;
    ctxt.node_limit = 2048;
    std::string fen = "r1bqkbnr/p4pp1/1pn1p2p/3p4/3P4/2NB1N2/PPP2PPP/R1BQ1RK1 w kq - 0 8";
    position pos(fen);
    move m = make_move(sq_d3, sq_f5);
    pos.do_move(m);
    think(pos, 0, ctxt);
    TEST_ASSERT(ctxt.pv[0].nr_of_moves > 0);
    TEST_EQ(2853, ctxt.pv[0].moves[0]);
    }

  void test_bug_4()
    {
    search_context ctxt;
    ctxt.node_limit = 4096;
    ctxt.max_depth = 15;
    ctxt.move_step = 5;
    std::string fen = "8/8/4K3/3Q4/6k1/8/4B3/r7 b - - 8 56";
    position pos(fen);
    think(pos, 1, ctxt);
    TEST_ASSERT(ctxt.pv[0].nr_of_moves > 0);
    TEST_EQ(1942, ctxt.pv[0].moves[0]);
    }

  void test_bug_5()
    {
    search_context ctxt;
    std::string fen = "7k/1P1R4/3Q4/8/B1P5/6P1/P4P2/RNB1K3 w Q - 5 33";
    ctxt.max_depth = 6;
    ctxt.move_step = 1;
    ctxt.node_limit = 4096;
    position pos(fen);
    think(pos, 0, ctxt);
    TEST_ASSERT(ctxt.pv[0].nr_of_moves > 0);
    TEST_EQ(31865, ctxt.pv[0].moves[0]);
    }

  void test_bug_6()
    {
    search_context ctxt;
    std::string fen = "rnbq1rk1/pppp1ppp/4pn2/8/1bPP4/2N5/PP1BPPPP/R2QKBNR w KQ - 4 5";
    ctxt.max_depth = max_ply;
    ctxt.move_step = 1;
    ctxt.node_limit = std::numeric_limits<uint64_t>::max();
    ctxt.time_limit = 5000;
    position pos(fen);
    think(pos, 1, ctxt);

    }
  }

void run_all_search_tests()
  {
  test_bug_1();
  test_bug_2();
  test_bug_3();
  test_bug_4();
  test_bug_5();
  //test_bug_6();
  }