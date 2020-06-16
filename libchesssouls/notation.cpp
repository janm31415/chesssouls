#include "notation.h"
#include "movegen.h"
#include "position.h"

namespace
  {
  static const char* piece_to_char[2] = { " PNBRQK", " pnbrqk" };
  }

char to_char(e_file f, bool tolower)
  {
  return char(f - file_a + (tolower ? 'a' : 'A'));
  }

char to_char(e_rank r)
  {
  return char(r - rank_1 + '1');
  }

const std::string to_string(e_square s)
  {
  char ch[] = { to_char(file_of(s)), to_char(rank_of(s)), 0 };
  return ch;
  }

std::string move_to_uci(move m)
  {
  e_square from = from_square(m);
  e_square to = to_square(m);

  if (m == move_none)
    return "(none)";

  if (m == move_null)
    return "0000";

  if (type_of(m) == castling)
    to = make_square(to > from ? file_g : file_c, rank_of(from));

  std::string str = to_string(from) + to_string(to);

  if (type_of(m) == promotion)
    str += piece_to_char[black][promotion_type(m)]; // Lower case

  return str;
  }

move parse_move(const position& pos, const std::string& str)
  {
  std::string input(str);
  if (input.length() == 5)
    input[4] = char(tolower(input[4]));

  movelist<legal> it(pos);
  for (; *it; ++it)
    {
    if (input == move_to_uci(*it))
      return *it;
    }

  return move_none;
  }