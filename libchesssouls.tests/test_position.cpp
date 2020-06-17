#include "test_position.h"
#include "test_assert.h"
#include <libchesssouls/position.h>

namespace
  {
  void test_fen()
    {
    std::string fen1("r1bqkb1r/ppp1pppp/2n2n2/3p4/2PP4/2N5/PP2PPPP/R1BQKBNR w KQkq - 0 100");
    position p(fen1);
    TEST_EQ(fen1, p.fen());

    std::string fen2("8/8/1KP5/3r4/8/8/8/k7 w - - 0 1");
    p.set_fen(fen2);
    TEST_EQ(fen2, p.fen());
    
    std::string fen3("8/p7/kpP5/qrp1b3/rpP2b2/pP4b1/P3K3/8 w - - 0 1");
    p.set_fen(fen3);
    TEST_EQ(fen3, p.fen());

    std::string fen4("8/8/2P5/4b3/5b2/6b1/4K3/k7 w - - 0 1");
    p.set_fen(fen4);
    TEST_EQ(fen4, p.fen());

    std::string fen5("8/5p1k/5np1/p2Pp3/3pP3/P2Q1PNq/7P/7K b - - 3 37");
    p.set_fen(fen5);
    TEST_EQ(fen5, p.fen());

    std::string fen6("r1bq1rk1/pppn1pbp/3p2p1/3Pp3/2P1P1n1/2N1BN2/PP2BPPP/R2QK2R w KQ - 3 9");
    p.set_fen(fen6);
    TEST_EQ(fen6, p.fen());

    std::string fen7("r1bq1rk1/ppp3bN/3p3n/2nP1p2/1PP1p3/2N4P/P2BBPP1/R2QK2R b KQ - 0 14");
    p.set_fen(fen7);
    TEST_EQ(fen7, p.fen());

    std::string fen8("3r1rk1/pQp2ppp/4b3/4b3/P3P3/2N5/1P4PP/2K2B1R b - a3 0 16");
    p.set_fen(fen8);
    TEST_EQ(std::string("3r1rk1/pQp2ppp/4b3/4b3/P3P3/2N5/1P4PP/2K2B1R b - - 0 16"), p.fen());

    std::string fen9("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    p.set_fen(fen9);
    TEST_EQ(fen9, p.fen());

    std::string fen10("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"); 
    p.set_fen(fen10);
    TEST_EQ(fen10, p.fen());

    std::string fen11("8/1K6/6Q1/1p6/8/8/2k1qp2/8 b - - 5 80");
    p.set_fen(fen11);
    TEST_EQ(fen11, p.fen());

    std::string fen12("7q/k2QP3/8/8/6K1/6P1/8/8 b - - 8 74");
    p.set_fen(fen12);
    TEST_EQ(fen12, p.fen());

    std::string fen13("r1b1k2r/ppq2pp1/3bpn1p/2p5/3P4/3B1N2/PPPB1PPP/R2Q1RK1 w kq - 0 13");
    p.set_fen(fen13);
    TEST_EQ(fen13, p.fen());

    std::string fen14("1rb2rk1/p4ppp/1p1qp1n1/3n2N1/2pP4/2P3P1/PPQ2PBP/R1B1R1K1 w - - 0 1");
    p.set_fen(fen14);
    TEST_EQ(fen14, p.fen());
    }
  }

void run_all_position_tests()
  {
  test_fen();
  }