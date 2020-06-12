#include "notation.h"

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