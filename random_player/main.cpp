#include <iostream>

#include <libchesssouls/bitboard.h>
#include <libchesssouls/hash.h>
#include <libchesssouls/position.h>
#include <libchesssouls/movegen.h>
#include <libchesssouls/notation.h>

#include <string>
#include <random>

#include <signal.h>



move generate_random_move(const position& pos)
  {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<> distrib(0, 1000);
  movelist<legal> moves(pos);
  size_t nr_of_moves = moves.size();
  if (nr_of_moves == 0)
    return move_none;
  size_t move_id = distrib(gen) % moves.size();
  for (size_t i = 0; i < move_id; ++i)
    ++moves;
  return *moves;
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

void xboard()
  {
  signal(SIGINT, SIG_IGN);
  std::cout << "\n";
  std::string fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  position pos(fen);
  char line[256], command[256];
  e_color computer_side = color_end;
  for (;;)
    {
    fflush(stdout);
    if (pos.side_to_move() == computer_side)
      {
      auto m = generate_random_move(pos);
      if (m == move_none)
        {
        computer_side = color_end;
        continue;
        }
      std::cout << "move " << move_to_uci(m) << "\n";
      pos.do_move(m);
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
      std::cout << pos.pretty();
      continue;
      }
    if (std::string(command) == std::string("st"))
      continue;
    if (std::string(command) == std::string("sd"))
      continue;
    if (std::string(command) == std::string("level"))
      continue;
    if (std::string(command) == std::string("time"))
      continue;
    if (std::string(command) == std::string("otim"))
      continue;
    if (std::string(command) == std::string("post"))
      continue;
    if (std::string(command) == std::string("nopost"))
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
  std::string fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  //std::string fen("4k3/RR6/8/8/8/8/8/4K3 w KQkq - 0 1");
  position pos(fen);

  e_color computer_side = color_end;
  for (;;)
    {
    if (pos.side_to_move() == computer_side)
      {
      auto m = generate_random_move(pos);
      if (m == move_none)
        {
        std::cout << "(no legal moves)\n";
        computer_side = color_end;
        continue;
        }
      pos.do_move(m);
      std::cout << pos.pretty();
      print_result(pos);
      continue;
      }
    std::cout << ":0> ";
    std::string line;
    std::cin >> line;
    if (line == std::string("on"))
      {
      computer_side = pos.side_to_move();
      continue;
      }
    if (line == std::string("off"))
      {
      computer_side = color_end;
      continue;
      }
    if (line == std::string("d"))
      {
      std::cout << pos.pretty();
      continue;
      }
    if (line == std::string("new"))
      {
      computer_side = color_end;
      pos.set_fen(fen);
      continue;
      }
    if (line == std::string("undo"))
      {
      computer_side = color_end;
      move m = pos.last_move();
      if (m != move_none)
        pos.undo_move(m);
      std::cout << pos.pretty();
      continue;
      }
    if (line == std::string("xboard"))
      {
      xboard();
      break;
      }
    if (line == std::string("exit"))
      break;
    move m = parse_move(pos, line);
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