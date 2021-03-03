#include <iostream>

#include <libchesssouls/bitboard.h>
#include <libchesssouls/book.h>
#include <libchesssouls/eval.h>
#include <libchesssouls/hash.h>
#include <libchesssouls/position.h>
#include <libchesssouls/movegen.h>
#include <libchesssouls/notation.h>
#include <libchesssouls/search.h>

#include <string>
#include <chrono>
#include <random>

#include <signal.h>

namespace
  {
  int maximum_moves = 1;
  int maximum_time = 1 << 25;
  int moves_left = 1;
  int time_left = 1 << 25;
  int time_inc = 1;
  }


move generate_move(position& pos, search_context& ctxt)
  {
  think(pos, 2, ctxt);
  if (ctxt.main_pv.nr_of_moves == 0)
    return move_none;
  return ctxt.main_pv.moves[0];
  }

void print_result(const position& pos)
  {
  if (movelist<legal>(pos).size() == 0)
    {
    if (pos.checkers())
      {
      if (pos.side_to_move() == white)
        std::cout << "0-1 {Black mates}\n";
      else
        std::cout << "1-0 {White mates}\n";
      }
    else
      {
      std::cout << "1/2-1/2 {Stalemate}\n";
      }
    }
  else if (pos.repetitions() >= 2)
    {
    std::cout << "1/2-1/2 {Draw by repetition}\n";
    }
  else if (pos.fifty() > 99)
    {
    std::cout << "1/2-1/2 {Draw by fifty move rule}\n";
    }
  }

int get_duration(const std::chrono::time_point<std::chrono::system_clock>& tick, const std::chrono::time_point<std::chrono::system_clock>& tock)
  {
  std::chrono::duration<double> elapsed = tock - tick;
  return int(elapsed.count()*1000.0);
  }

void xboard()
  {
  signal(SIGINT, SIG_IGN);
  std::cout << "\n";
  std::string fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  position pos(fen);
  char line[256], command[256];
  e_color computer_side = color_end;
  search_context ctxt;
  for (;;)
    {
    fflush(stdout);
    if (pos.side_to_move() == computer_side)
      {
      auto tick = std::chrono::system_clock::now();
      if (maximum_moves <= 1)
        ctxt.time_limit = time_left;
      else
        {
        ctxt.time_limit = (time_left * 95 / 100 - 100 + time_inc * (moves_left - 1)) / moves_left;
        if (ctxt.time_limit < 0)
          ctxt.time_limit = 0;
        }
      auto m = generate_move(pos, ctxt);
      if (m == move_none)
        {
        computer_side = color_end;
        continue;
        }
      std::cout << "move " << move_to_uci(m) << "\n";
      auto tock = std::chrono::system_clock::now();
      pos.do_move(m);
      time_left -= get_duration(tick, tock);
      --moves_left;
      if (moves_left <= 0)
        {
        moves_left = maximum_moves;
        if (maximum_moves <= 1)
          time_left = maximum_time;
        else
          time_left += maximum_time;
        }
      continue;
      }
    if (!fgets(line, 256, stdin))
      return;
    if (line[0] == '\n')
      continue;
    sscanf(line, "%s", command);

    if (std::string(command) == std::string("xboard"))
      {
      continue;
      }
    if (std::string(command) == std::string("protover"))
      {
      std::cout << "feature myname=\" Chess Souls 1.0 \"\n";
      std::cout << "feature done=0\n";
      std::cout << "feature ping = 1\n";
      std::cout << "feature memory=1\n";
      std::cout << "feature setboard=1\n";
      std::cout << "feature variants=\"normal\"\n";
      std::cout << "feature done=1\n";
      continue;
      }
    if (std::string(command) == std::string("go"))
      {
      computer_side = pos.side_to_move();
      continue;
      }
    if (std::string(command) == std::string("force"))
      {
      computer_side = color_end;
      continue;
      }
    if (std::string(command) == std::string("new"))
      {
      computer_side = black;
      pos.set_fen(fen);
      continue;
      }
    if (std::string(command) == std::string("undo"))
      {
      computer_side = color_end;
      move m = pos.last_move();
      if (m != move_none)
        pos.undo_move(m);
      continue;
      }
    if (std::string(command) == std::string("remove"))
      {
      if (pos.ply() >= 2)
        {
        move m = pos.last_move();
        if (m != move_none)
          pos.undo_move(m);
        m = pos.last_move();
        if (m != move_none)
          pos.undo_move(m);
        }
      continue;
      }
    if (std::string(command) == std::string("white"))
      {
      pos.set_side_to_move(white);
      computer_side = black;
      continue;
      }
    if (std::string(command) == std::string("black"))
      {
      pos.set_side_to_move(black);
      computer_side = white;
      continue;
      }
    if (std::string(command) == std::string("ping"))
      {
      int nr = 0;
      sscanf(line, "ping %d", &nr);
      std::cout << "pong " << nr << "\n";
      continue;
      }
    if (std::string(command) == std::string("st"))
      {
      sscanf(line, "st %d", &maximum_time);
      maximum_time *= 1000;
      maximum_moves = 1;
      time_left = maximum_time;
      moves_left = 1;
      time_inc = 0;
      ctxt.max_depth = max_ply;
      continue;
      }
    if (std::string(command) == std::string("sd"))
      {
      sscanf(line, "sd %d", &ctxt.max_depth);
      ctxt.time_limit = 1 << 25;
      continue;
      }
    if (std::string(command) == std::string("level"))
      {
      int sec = 0;
      if (sscanf(line, "level %d %d %d",
        &maximum_moves, &maximum_time, &time_inc) == 3 ||
        sscanf(line, "level %d %d:%d %d",
          &maximum_moves, &maximum_time, &sec, &time_inc) == 4)
        {
        moves_left = maximum_moves;
        time_left = maximum_time = 60000 * maximum_time + 1000 * sec;
        time_inc *= 1000;
        }
      continue;
      }
    if (std::string(command) == std::string("time"))
      {
      sscanf(line, "time %d", &time_left);
      time_left *= 10;
      ctxt.max_depth = max_ply;
      continue;
      }
    if (std::string(command) == std::string("otim"))
      continue;
    if (std::string(command) == std::string("post"))
      continue;
    if (std::string(command) == std::string("nopost"))
      continue;
    if (std::string(command) == std::string("easy"))
      continue;
    if (std::string(command) == std::string("hard"))
      continue;
    if (std::string(command) == std::string("computer"))
      continue;
    if (std::string(command) == std::string("quit"))
      return;
    move m = parse_move(pos, std::string(command));
    if (m == move_none)
      {
      std::cout << "Illegal move.\n";
      }
    else
      {
      pos.do_move(m);
      }
    }
  }

int main(int argc, char** argv)
  {
  init_bitboards();
  init_hash();
  init_eval();
  read_book("");
  std::string fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  position pos(fen);
  search_context ctxt;
  ctxt.max_depth = 5;
  //ctxt.use_book = false;

  char line[256], command[256];
  e_color computer_side = color_end;
  for (;;)
    {
    if (pos.side_to_move() == computer_side)
      {
      auto tick = std::chrono::system_clock::now();
      if (maximum_moves <= 1)
        ctxt.time_limit = time_left;
      else
        {
        ctxt.time_limit = (time_left * 95 / 100 - 100 + time_inc * (moves_left - 1)) / moves_left;
        if (ctxt.time_limit < 0)
          ctxt.time_limit = 0;
        }
      if (ctxt.time_limit > (maximum_time / maximum_moves) * 2)
        ctxt.time_limit = (maximum_time / maximum_moves) * 2;
      auto m = generate_move(pos, ctxt);
      if (m == move_none)
        {
        std::cout << "(no legal moves)\n";
        computer_side = color_end;
        continue;
        }
      pos.do_move(m);
      std::cout << pos.pretty();
      auto tock = std::chrono::system_clock::now();
      print_result(pos);
      time_left -= get_duration(tick, tock);
      --moves_left;
      if (moves_left <= 0)
        {
        moves_left = maximum_moves;
        if (maximum_moves <= 1)
          time_left = maximum_time;
        else
          time_left += maximum_time;
        }
      continue;
      }
    std::cout << ":0> ";
    if (!fgets(line, 256, stdin))
      {
      std::cout << "Error: cannot read from stdin\n";
      return -1;
      }
    if (line[0] == '\n')
      continue;
    sscanf(line, "%s", command);
    if (std::string(command) == std::string("on"))
      {
      computer_side = pos.side_to_move();
      continue;
      }
    if (std::string(command) == std::string("off"))
      {
      computer_side = color_end;
      continue;
      }
    if (std::string(command) == std::string("d"))
      {
      std::cout << pos.pretty();
      continue;
      }
    if (std::string(command) == std::string("new"))
      {
      computer_side = color_end;
      pos.set_fen(fen);
      continue;
      }
    if (std::string(command) == std::string("undo"))
      {
      computer_side = color_end;
      move m = pos.last_move();
      if (m != move_none)
        pos.undo_move(m);
      std::cout << pos.pretty();
      continue;
      }
    if (std::string(command) == std::string("st"))
      {
      sscanf(line, "st %d", &maximum_time);
      maximum_time *= 1000;
      maximum_moves = 1;
      time_left = maximum_time;
      moves_left = 1;
      ctxt.max_depth = max_ply;
      continue;
      }
    if (std::string(command) == std::string("sd"))
      {
      sscanf(line, "sd %d", &ctxt.max_depth);
      ctxt.time_limit = 1 << 25;
      continue;
      }
    if (std::string(command) == std::string("xboard"))
      {
      xboard();
      break;
      }
    if (std::string(command) == std::string("exit"))
      break;
    move m = parse_move(pos, std::string(command));
    if (m == move_none)
      {
      std::cout << "Illegal move.\n";
      }
    else
      {
      pos.do_move(m);
      std::cout << pos.pretty();
      print_result(pos);
      }
    }

  return 0;
  }