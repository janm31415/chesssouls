#include "test.h"
#include "position.h"
#include "trans.h"
#include "search.h"
#include "book.h"
#include "eval.h"
#include "pawn.h"
#include "king.h"
#include "eval_table.h"
#include "movegen.h"
#include "notation.h"

#include <fstream>
#include <iostream>
#include <vector>

namespace
  {
  struct test_pos
    {
    std::string fen_pos;
    std::vector<std::string> move_list;
    std::string id;
    bool best;
    };

  struct test_result
    {
    bool succeeded;
    std::string suggested_move;
    int64_t nodes;
    };

  char piece_to_char[7] = { 0, 'P', 'N', 'B', 'R', 'Q', 'K' };


  void ConvertMoveToInternalMove(test_pos& t)
    {
    position pos(t.fen_pos);
    for (auto& move : t.move_list)
      {
      movelist<legal> moves(pos);
      for (; *moves; ++moves)
        {
        if (move_to_uci(*moves) == move)
          break; // found the move and it is written the same
        if (type_of(*moves) != castling)
          {
          e_piecetype pt = type_of(pos.piece_on(from_square(*moves)));
          if (pt != pawn)
            {
            if (move.front() != piece_to_char[(int)pt])
              continue;
            }
          else
            {
            bool pawn_move = true;
            for (int i = 1; i < 7; ++i)
              {
              if (move.front() == piece_to_char[i])
                {
                pawn_move = false;
                break;
                }
              }
            if (!pawn_move)
              continue;
            }
          e_square target_sq = to_square(*moves);
          e_file f = file_of(target_sq);
          e_rank r = rank_of(target_sq);
          std::string tsq;
          tsq.push_back(to_char(f));
          tsq.push_back(to_char(r));
          if (move.find(tsq) == std::string::npos)
            continue;
          printf("Move %s equals %s\n", move.c_str(), move_to_uci(*moves).c_str());
          move = move_to_uci(*moves);
          break;
          }
        }
      }
    }

  std::vector<std::string> split(std::string moves)
    {
    std::vector<std::string> out;
    auto pos = moves.find(' ');
    while (pos != std::string::npos)
      {
      out.push_back(moves.substr(0, pos));
      moves = moves.substr(pos + 1);
      pos = moves.find(' ');
      }
    if (!moves.empty())
      out.push_back(moves);
    return out;
    }

  test_pos ReadEPDRecord(std::string epd)
    {
    test_pos t;
    t.best = true;
    auto pos = epd.find("bm ");
    if (pos == std::string::npos)
      {
      pos = epd.find("am ");
      t.best = false;
      }
    t.fen_pos = epd.substr(0, pos).append("0 1");
    epd = epd.substr(pos + 3);
    auto pos2 = epd.find_first_of(';');
    std::string moves = epd.substr(0, pos2);
    t.move_list = split(moves);
    epd = epd.substr(pos2 + 1);
    pos = epd.find_first_of('"');
    pos2 = epd.find_last_of('"');
    t.id = epd.substr(pos + 1, pos2 - pos - 1);
    ConvertMoveToInternalMove(t);
    return t;
    }

  std::vector<test_pos> GenerateTestFromFile(const std::string& filename)
    {
    std::vector<test_pos> test;
    std::ifstream f(filename);
    if (f.is_open())
      {
      while (!f.eof())
        {
        std::string l;
        std::getline(f, l);
        test.push_back(ReadEPDRecord(l));
        }
      f.close();
      }

    return test;
    }

  } // namespace
int WinAtChess_test()
  {
  return test_epd("C:/_Dev/chesssouls_orig/Chess/ChessSouls/WAC.txt");
  }

int Kaufman_test()
  {
  return test_epd("C:/_Dev/chesssouls_orig/Chess/ChessSouls/Kaufman.txt");
  }

int BratkoKopec_test()
  {
  return test_epd("C:/_Dev/chesssouls_orig/Chess/ChessSouls/BK.txt");
  }

int test_epd(const char* filename)
  {
  int score = 0;
  auto test = GenerateTestFromFile(filename);
  int count = 0;
  uint64_t total_nodes = 0;
  std::vector<test_result> results;
  for (auto t : test)
    {
    std::cout << "Score so far: " << score << " / " << count << std::endl;
    std::cout << "Now working on problem " << t.id << " (" << ++count << " of " << test.size() << ")" << std::endl;

    position pos(t.fen_pos);
    search_context ctxt;
    ctxt.use_book = false;
    ctxt.max_depth = 10;
    ctxt.time_limit = 1 << 25;
    std::cout << pos.pretty();
    //print_eval(std::cout, pos);
    clear_transposition_table();
    clear_pawn_table();
    clear_eval_table();
    clear_king_table();
    think(pos, 1, ctxt);
    total_nodes += nodes;
    test_result res;
    res.nodes = nodes;
    res.suggested_move = move_to_uci(ctxt.main_pv.moves[0]);
    res.succeeded = false;
    if (t.best)
      {
      if (t.move_list.size() == 1)
        std::cout << "I found " << move_to_uci(ctxt.main_pv.moves[0]) << " and the best move was " << t.move_list[0] << std::endl;
      else if (t.move_list.size() > 1)
        {
        std::cout << "I found " << move_to_uci(ctxt.main_pv.moves[0]) << " and the best moves were " << std::endl;
        for (const auto& m : t.move_list)
          std::cout << "  " << m << std::endl;
        }
      for (const auto& m : t.move_list)
        {
        if (move_to_uci(ctxt.main_pv.moves[0]) == m)
          {
          res.succeeded = true;
          ++score;
          }
        }
      }
    else
      {
      if (t.move_list.size() == 1)
        std::cout << "I found " << move_to_uci(ctxt.main_pv.moves[0]) << " and the move to avoid was " << t.move_list[0] << std::endl;
      else if (t.move_list.size() > 1)
        {
        std::cout << "I found " << move_to_uci(ctxt.main_pv.moves[0]) << " and the moves to avoid were " << std::endl;
        for (const auto& m : t.move_list)
          std::cout << "  " << m << std::endl;
        }
      bool ok = true;
      for (const auto& m : t.move_list)
        {
        if (move_to_uci(ctxt.main_pv.moves[0]) == m)
          {
          ok = false;
          }
        }
      if (!ok)
        {
        res.succeeded = true;
        ++score;
        }
      }
    results.push_back(res);
    std::cout << "Nodes searched so far: " << total_nodes << std::endl;
    }

  for (size_t i = 0; i < test.size(); ++i)
    {
    std::cout << test[i].id << ": " << (results[i].succeeded ? "SUCCESS" : "FAIL");
    if (test[i].best)
      std::cout << ": expected ";
    else
      std::cout << ": to avoid ";
    for (const auto& m : test[i].move_list)
      std::cout << m << ", ";
    std::cout << "actual " << results[i].suggested_move << std::endl;
    }
  std::cout << "Total score: " << score << " / " << test.size() << std::endl;
  std::cout << "Total nodes searched: " << total_nodes << std::endl;
  return score;
  }