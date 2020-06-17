#include "test_assert.h"
#include "test_movegen.h"
#include "test_position.h"
#include "test_search.h"

#include <libchesssouls/bitboard.h>
#include <libchesssouls/eval.h>
#include <libchesssouls/hash.h>

#include <ctime>

int main(int /*argc*/, const char* /*argv*/[])
  {
  InitTestEngine();

  init_bitboards();
  init_hash();
  init_eval();

  auto tic = std::clock();
  run_all_search_tests();
  run_all_position_tests();
  run_all_movegen_tests();  
  auto toc = std::clock();

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