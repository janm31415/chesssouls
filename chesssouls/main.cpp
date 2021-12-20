#include <iostream>

#include <libchesssouls/bitboard.h>
#include <libchesssouls/book.h>
#include <libchesssouls/eval.h>
#include <libchesssouls/hash.h>
#include <libchesssouls/position.h>
#include <libchesssouls/movegen.h>
#include <libchesssouls/notation.h>
#include <libchesssouls/search.h>
#include <libchesssouls/trans.h>

#include <string>
#include <chrono>
#include <random>
#include <sstream>

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

int main(int argc, char** argv)
  {
  FILE* logging = fopen("C:/tmp/chesssoulslogging.txt", "w");
  if (!logging)
    printf("Cannot open LOG file chesssoulslogging.txt\n");
  init_bitboards();
  init_hash();
  init_eval();
  init_transposition_table();
  read_book("");
  std::string fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  //std::string fen("3q4/7p/8/5K2/3k4/8/8/8 b - - 31 140");
  position pos(fen);
  search_context ctxt;
  ctxt.max_depth = 5;
  //ctxt.use_book = false;
  bool xboard = false;
  char command_line[256], command[256];
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
        fflush(stdout);
        computer_side = color_end;
        continue;
        }
      if (xboard)
        {
        std::cout << "move " << move_to_uci(m) << "\n";
        fflush(stdout);
        }
      if (logging)
        {
        std::stringstream ss;
        ss << "move " << move_to_uci(m) << "\n";
        fprintf(logging, ss.str().c_str());
        fflush(logging);
        }
      pos.do_move(m);
      if (!xboard)
        {
        std::cout << pos.pretty();
        print_result(pos);
        }
      auto tock = std::chrono::system_clock::now();      
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
    if (!xboard)
      {
      std::cout << ":0> ";
      }
    if (!fgets(command_line, 256, stdin))
      {
      std::cout << "Error: cannot read from stdin\n";
      return -1;
      }
    if (logging)
      {
      fprintf(logging, command_line);
      fflush(logging);
      }
    if (command_line[0] == '\n')
      continue;
    sscanf(command_line, "%s", command);
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
    if (std::string(command) == std::string("d"))
      {
      std::cout << pos.pretty();
      print_result(pos);
      fflush(stdout);
      continue;
      }
    if (std::string(command) == std::string("fen"))
      {
      std::cout << pos.fen() << "\n";
      fflush(stdout);
      continue;
      }
    if (std::string(command) == std::string("new"))
      {
      computer_side = black;
      pos.set_fen(fen);
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
    if (std::string(command) == std::string("protover"))
      {
      std::cout << "feature myname=\" Chess Souls 1.0 \"\n";
      std::cout << "feature done=0\n";
      std::cout << "feature ping=1\n";
      std::cout << "feature memory=1\n";
      std::cout << "feature setboard=1\n";
      std::cout << "feature variants=\"normal\"\n";
      std::cout << "feature done=1\n";
      fflush(stdout);
      continue;
      }
    if (std::string(command) == std::string("setboard"))
      {
      std::string mess(command_line);
      if (mess.back() == '\n')
        mess.pop_back();
      computer_side = color_end;
      std::string newfen(mess.begin() + 8, mess.end());
      while (!newfen.empty() && newfen.front() == ' ')
        newfen.erase(newfen.begin());
      if (!newfen.empty() && newfen.back() == '\n')
        newfen.pop_back();
      pos.set_fen(newfen);
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
      sscanf(command_line, "ping %d", &nr);
      std::cout << "pong " << nr << "\n";
      fflush(stdout);
      continue;
      }
    if (std::string(command) == std::string("level"))
      {
      int sec = 0;
      if (sscanf(command_line, "level %d %d %d",
        &maximum_moves, &maximum_time, &time_inc) == 3 ||
        sscanf(command_line, "level %d %d:%d %d",
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
      sscanf(command_line, "time %d", &time_left);
      time_left *= 10;
      ctxt.max_depth = max_ply;
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
      sscanf(command_line, "st %d", &maximum_time);
      maximum_time *= 1000;
      maximum_moves = 1;
      time_left = maximum_time;
      moves_left = 1;
      ctxt.max_depth = max_ply;
      continue;
      }
    if (std::string(command) == std::string("sd"))
      {
      sscanf(command_line, "sd %d", &ctxt.max_depth);
      ctxt.time_limit = 1 << 25;
      continue;
      }
    if (std::string(command) == std::string("xboard"))
      {
      xboard = true;
      continue;
      }
    if (std::string(command) == std::string("accepted"))
      continue;
    if (std::string(command) == std::string("rejected"))
      continue;
    if (std::string(command) == std::string("memory"))
      continue;
    if (std::string(command) == std::string("random"))
      continue;
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
      break;
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
      if (!xboard)
        {
        std::cout << pos.pretty();
        print_result(pos);
        }
      }
    }
  destroy_transposition_table();
  if (logging)
    fclose(logging);
  return 0;
  }