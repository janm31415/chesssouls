#include "test_movegen.h"
#include <libchesssouls/movegen.h>
#include <libchesssouls/perft.h>
#include <libchesssouls/position.h>
#include <string>
#include "test_assert.h"

#include <iostream>

namespace
  {

  void test_generate_moves_starting_pos()
    {
    std::string fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    position p(fen);
    movelist<legal> moves(p);

    TEST_EQ(20, moves.size());
    }

  void test_perft()
    {
    std::string fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    position p(fen);
    TEST_EQ(20, perft(p, 1));
    TEST_EQ(400, perft(p, 2));
    TEST_EQ(8902, perft(p, 3));
    TEST_EQ(197281, perft(p, 4));
    TEST_EQ(4865609, perft(p, 5));

    //TEST_EQ(119060324, perft(p, 6));

    //TEST_EQ(119060324, perft(p, 7));
    }

  void test_bug_1()
    {
    std::string fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    position p(fen);
    p.do_move(723);

    p.do_move(3567);
    TEST_ASSERT(p.position_is_ok());
    p.do_move(175);
    TEST_ASSERT(p.position_is_ok());
    p.undo_move(175);
    TEST_ASSERT(p.position_is_ok());
    p.undo_move(3567);
    TEST_ASSERT(p.position_is_ok());

    p.do_move(3567);
    p.do_move(267);
    TEST_ASSERT(p.position_is_ok());
    p.undo_move(267);
    TEST_ASSERT(p.position_is_ok());
    p.undo_move(3567);
    TEST_ASSERT(p.position_is_ok());
    }
  
  void test_bug_2()
    {
    std::string fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    position p(fen);
    p.do_move(658);
    p.do_move(3112);
    p.do_move(216);
    movelist<legal> moves(p);
    TEST_EQ(17, moves.size());
    }
    
  void test_kiwipete()
    {
    std::string fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    position p(fen);
    TEST_EQ(48, perft(p, 1));
    TEST_EQ(2039, perft(p, 2));
    TEST_EQ(97862, perft(p, 3));
    TEST_EQ(4085603, perft(p, 4));
    //TEST_EQ(193690690, perft(p, 5));
    }

  void test_position_3()
    {
    std::string fen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ");
    position p(fen);
    TEST_EQ(14, perft(p, 1));
    TEST_EQ(191, perft(p, 2));
    TEST_EQ(2812, perft(p, 3));
    TEST_EQ(43238, perft(p, 4));
    TEST_EQ(674624, perft(p, 5));
    TEST_EQ(11030083, perft(p, 6));
    //TEST_EQ(178633661, perft(p, 7));
    }

  void test_bug_3()
    {
    std::string fen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ");
    position p(fen);
    p.do_move(1601);
    p.do_move(1877);
    p.do_move(2073);
    p.do_move(3234);
    p.do_move(34922);
    TEST_ASSERT(p.position_is_ok());
    p.undo_move(34922);
    TEST_ASSERT(p.position_is_ok());
    }

  void test_position_4()
    {
    std::string fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    position p(fen);
    TEST_EQ(6, perft(p, 1));
    TEST_EQ(264, perft(p, 2));
    TEST_EQ(9467, perft(p, 3));
    TEST_EQ(422333, perft(p, 4));
    //TEST_EQ(15833292, perft(p, 5));
    }
  }


void run_all_movegen_tests()
  {
  test_generate_moves_starting_pos();
  test_perft();
  test_bug_1();
  test_bug_2();
  test_kiwipete();
  test_position_3();
  test_position_4();
  test_bug_3();
  }