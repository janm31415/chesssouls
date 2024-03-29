#include "test_assert.h"
#include "test_eval.h"
#include "test_movegen.h"
#include "test_position.h"
#include "test_search.h"

#include <libchesssouls/bitboard.h>
#include <libchesssouls/eval.h>
#include <libchesssouls/hash.h>
#include <libchesssouls/trans.h>
#include <libchesssouls/eval_table.h>
#include <libchesssouls/pawn.h>
#include <libchesssouls/king.h>

#include <ctime>

int main(int /*argc*/, const char* /*argv*/[])
  {
  InitTestEngine();

  init_bitboards();
  init_hash();
  init_eval();
  init_transposition_table();
  init_king_table();
  init_pawn_table();
  init_eval_table();

  auto tic = std::clock();
  run_all_eval_tests();
  //run_all_search_tests();
  //run_all_position_tests();
  //run_all_movegen_tests();  
  auto toc = std::clock();

  destroy_transposition_table();
  destroy_eval_table();
  destroy_pawn_table();
  destroy_king_table();

  if (!testing_fails) 
    {
    TEST_OUTPUT_LINE("Succes: %d tests passed.", testing_success);
    }
  else 
    {
    TEST_OUTPUT_LINE("FAILURE: %d out of %d tests failed (%d failures).", testing_fails, testing_success+testing_fails, testing_fails);
    }
  TEST_OUTPUT_LINE("Test time: %f seconds.", (double)(toc - tic)/(double)CLOCKS_PER_SEC);
  return CloseTestEngine(true);
  }