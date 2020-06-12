#include "position.h"
#include "notation.h"
#include <algorithm>
#include <sstream>

namespace
  {
  static const std::string piece_to_char(" PNBRQK   pnbrqk");
  }

position::position()
  {

  }

position::position(const std::string& fen)
  {
  set_fen(fen);
  }

position::~position()
  {

  }

void position::clear()
  {
  std::memset(this, 0, sizeof(position));
  }

void position::set_fen(const std::string& fen)
  {
  /*
  A FEN string defines a particular position using only the ASCII character set.

   A FEN string contains six fields separated by a space. The fields are:

   1) Piece placement (from white's perspective). Each rank is described, starting
      with rank 8 and ending with rank 1. Within each rank, the contents of each
      square are described from file A through file H. Following the Standard
      Algebraic Notation (SAN), each piece is identified by a single letter taken
      from the standard English names. White pieces are designated using upper-case
      letters ("PNBRQK") whilst Black uses lowercase ("pnbrqk"). Blank squares are
      noted using digits 1 through 8 (the number of blank squares), and "/"
      separates ranks.

   2) Active color. "w" means white moves next, "b" means black.

   3) Castling availability. If neither side can castle, this is "-". Otherwise,
      this has one or more letters: "K" (White can castle kingside), "Q" (White
      can castle queenside), "k" (Black can castle kingside), and/or "q" (Black
      can castle queenside).

   4) En passant target square (in algebraic notation). If there's no en passant
      target square, this is "-". If a pawn has just made a 2-square move, this
      is the position "behind" the pawn. This is recorded regardless of whether
      there is a pawn in position to make an en passant capture.

   5) Halfmove clock. This is the number of halfmoves since the last pawn advance
      or capture. This is used to determine if a draw can be claimed under the
      fifty-move rule.

   6) Fullmove number. The number of the full move. It starts at 1, and is
      incremented after Black's move.
  */
  unsigned char col, row, token;
  size_t idx;
  e_square sq = sq_a8;
  clear();
  std::istringstream ss(fen);
  ss >> std::noskipws;
  // 1. Piece placement
  while ((ss >> token) && !isspace(token))
    {
    if (isdigit(token))
      sq += e_square(token - '0'); // Advance the given number of files

    else if (token == '/')
      sq -= e_square(16);

    else if ((idx = piece_to_char.find(token)) != std::string::npos)
      {
      put_piece(sq, color_of(e_piece(idx)), type_of(e_piece(idx)));
      ++sq;
      }
    }

  // 2. Active color
  ss >> token;
  side_to_move = (token == 'w' ? white : black);
  ss >> token;

  // 3. Castling availability.
  while ((ss >> token) && !isspace(token))
    {
    switch (token)
      {
      case 'K':
        castle |= 1;
        break;
      case 'Q':
        castle |= 2;
        break;
      case 'k':
        castle |= 4;
        break;
      case 'q':
        castle |= 8;
        break;
      }
    }
  
  // 4. En passant square.
  ep = sq_none;
  if (((ss >> col) && (col >= 'a' && col <= 'h'))
    && ((ss >> row) && (row == '3' || row == '6')))
    {
    ep = make_square(e_file(col - 'a'), e_rank(row - '1'));
    }

  // 5-6. Halfmove clock and fullmove number
  ss >> std::skipws >> rule50 >> game_ply;

  game_ply = std::max(2 * (game_ply - 1), 0) + (side_to_move == black);
  }

std::string position::fen() const
  {
  std::ostringstream ss;
  int empty_count;

  for (e_rank r = rank_8; r >= rank_1; --r)
    {
    for (e_file f = file_a; f <= file_h; ++f)
      {
      for (empty_count = 0; f <= file_h && empty(make_square(f, r)); ++f)
        ++empty_count;

      if (empty_count)
        ss << empty_count;

      if (f <= file_h)
        ss << piece_to_char[piece_on(make_square(f, r))];
      }

    if (r > rank_1)
      ss << '/';
    }

  ss << (side_to_move == white ? " w " : " b ");

  if (castle & 1)
    ss << 'K';
  if (castle & 2)
    ss << 'Q';
  if (castle & 4)
    ss << 'k';
  if (castle & 8)
    ss << 'q';
  if (!(castle & 15))
    ss << '-';

  ss << (ep == sq_none ? " - " : " " + to_string(ep) + " ")
    << rule50 << " " << 1 + (game_ply - (side_to_move == black)) / 2;


  return ss.str();
  }

void position::put_piece(e_square s, e_color c, e_piecetype pt)
  {
  board[s] = make_piece(c, pt);
  bb_by_type[all_pieces] |= s;
  bb_by_type[pt] |= s;
  bb_by_color[c] |= s;
  }

void position::remove_piece(e_square s, e_color c, e_piecetype pt)
  {
  board[s] = no_piece;
  bb_by_type[all_pieces] ^= s;
  bb_by_type[pt] ^= s;
  bb_by_color[c] ^= s;
  }

bool position::empty(e_square s) const
  {
  //return (bb_by_type[all_pieces] & s) == 0;
  return board[s] == no_piece;
  }

e_piece position::piece_on(e_square s) const
  {
  return board[s];
  }