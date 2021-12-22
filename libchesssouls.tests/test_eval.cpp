#include "test_eval.h"
#include "test_assert.h"

#include <libchesssouls/movegen.h>
#include <libchesssouls/movepick.h>
#include <libchesssouls/perft.h>
#include <libchesssouls/position.h>
#include <libchesssouls/search.h>
#include <libchesssouls/eval.h>
#include <libchesssouls/test.h>

#include <iostream>

namespace
  {
  void test_eval_1()
    {
    std::string fen = "r1b1k1nr/p1b4p/4p1p1/3p1p2/K3p3/8/P1P2q1P/8 b kq - 1 22";
    //std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    //std::string fen = "p6k/1p6/1p6/3p4/3P4/1P6/1P6/P6K w - 0 1";
    position pos(fen);
    int v = eval(pos);
    //TEST_EQ(v, 0);
    print_eval(std::cout, pos);
    }

  void test_wac()
    {
    //int score = WinAtChess_test();
    //int score = Kaufman_test();
    //int score = BratkoKopec_test();
    //int score = CCR_test();
    int score = silent_but_deadly_test();
    }
  }


void run_all_eval_tests()
  {
  //test_eval_1();
  test_wac();
  }