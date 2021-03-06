#include "position.h"
#include "movegen.h"
#include "notation.h"
#include "hash.h"
#include "eval.h"
#include <algorithm>
#include <sstream>
#include <intrin.h>
#include <iomanip>

namespace
  {
  static const std::string piece_to_char(" PNBRQK  pnbrqk");
  /*
  int castle_mask[64] = {
   7, 15, 15, 15,  3, 15, 15, 11,
  15, 15, 15, 15, 15, 15, 15, 15,
  15, 15, 15, 15, 15, 15, 15, 15,
  15, 15, 15, 15, 15, 15, 15, 15,
  15, 15, 15, 15, 15, 15, 15, 15,
  15, 15, 15, 15, 15, 15, 15, 15,
  15, 15, 15, 15, 15, 15, 15, 15,
  13, 15, 15, 15, 12, 15, 15, 14
    };
  */
  int castle_mask[64] = {
13, 15, 15, 15, 12, 15, 15, 14,
15, 15, 15, 15, 15, 15, 15, 15,
15, 15, 15, 15, 15, 15, 15, 15,
15, 15, 15, 15, 15, 15, 15, 15,
15, 15, 15, 15, 15, 15, 15, 15,
15, 15, 15, 15, 15, 15, 15, 15,
15, 15, 15, 15, 15, 15, 15, 15,
7, 15, 15, 15,  3, 15, 15, 11
    };
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
  ep = sq_none;
  for (int i = 0; i < nr_piecetype; ++i)
    for (int j = 0; j < 16; ++j)
      piece_list[white][i][j] = piece_list[black][i][j] = sq_none;
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
  _side_to_move = (token == 'w' ? white : black);
  ss >> token;

  // 3. Castling availability.
  while ((ss >> token) && !isspace(token))
    {
    switch (token)
      {
      case 'K':
        _castle |= 1;
        break;
      case 'Q':
        _castle |= 2;
        break;
      case 'k':
        _castle |= 4;
        break;
      case 'q':
        _castle |= 8;
        break;
      }
    }

  // 4. En passant square.
  ep = sq_none;
  if (((ss >> col) && (col >= 'a' && col <= 'h'))
    && ((ss >> row) && (row == '3' || row == '6')))
    {
    ep = make_square(e_file(col - 'a'), e_rank(row - '1'));

    if (!(attackers_to(ep) & pieces(_side_to_move, pawn)))
      ep = sq_none;
    }

  // 5-6. Halfmove clock and fullmove number
  ss >> std::skipws >> rule50 >> game_ply;

  game_ply = std::max(2 * (game_ply - 1), 0) + (_side_to_move == black);

  set_hash();
  set_lazy_material();

  compute_checkers();

  assert(position_is_ok());
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

  ss << (_side_to_move == white ? " w " : " b ");

  if (_castle & 1)
    ss << 'K';
  if (_castle & 2)
    ss << 'Q';
  if (_castle & 4)
    ss << 'k';
  if (_castle & 8)
    ss << 'q';
  if (!(_castle & 15))
    ss << '-';

  ss << (ep == sq_none ? " - " : " " + to_string(ep) + " ")
    << rule50 << " " << 1 + (game_ply - (_side_to_move == black)) / 2;

  return ss.str();
  }

void position::put_piece(e_square s, e_color c, e_piecetype pt)
  {
  board[s] = make_piece(c, pt);
  bb_by_type[all_pieces] |= s;
  bb_by_type[pt] |= s;
  bb_by_color[c] |= s;
  index[s] = piece_count[c][pt]++;
  piece_list[c][pt][index[s]] = s;
  }

void position::move_piece(e_square from, e_square to, e_color c, e_piecetype pt)
  {
  assert(pt != no_piecetype);
  bitboard from_to = square[from] ^ square[to];
  bb_by_type[all_pieces] ^= from_to;
  bb_by_type[pt] ^= from_to;
  bb_by_color[c] ^= from_to;
  board[from] = no_piece;
  board[to] = make_piece(c, pt);
  index[to] = index[from];
  piece_list[c][pt][index[to]] = to;
  }

void position::remove_piece(e_square s, e_color c, e_piecetype pt)
  {
  board[s] = no_piece;
  bb_by_type[all_pieces] ^= s;
  bb_by_type[pt] ^= s;
  bb_by_color[c] ^= s;
  e_square last_square = piece_list[c][pt][--piece_count[c][pt]];
  index[last_square] = index[s];
  piece_list[c][pt][index[last_square]] = last_square;
  piece_list[c][pt][piece_count[c][pt]] = sq_none;
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

e_piece position::moved_piece(move m) const
  {
  return board[from_square(m)];
  }

bitboard position::pieces() const
  {
  return bb_by_type[all_pieces];
  }

bitboard position::pieces(e_color c) const
  {
  return bb_by_color[c];
  }

bitboard position::pieces(e_piecetype pt) const
  {
  return bb_by_type[pt];
  }

bitboard position::pieces(e_piecetype pt1, e_piecetype pt2) const
  {
  return bb_by_type[pt1] | bb_by_type[pt2];
  }

bitboard position::pieces(e_color c, e_piecetype pt) const
  {
  return bb_by_color[c] & bb_by_type[pt];
  }

bitboard position::pieces(e_color c, e_piecetype pt1, e_piecetype pt2) const
  {
  return bb_by_color[c] & (bb_by_type[pt1] | bb_by_type[pt2]);
  }

bitboard position::attackers_to(e_square s) const
  {
  return attackers_to(s, bb_by_type[all_pieces]);
  }

bitboard position::attackers_to(e_square s, bitboard occ) const
  {
  return  (attacks_from_pawn(s, black) & pieces(white, pawn))
    | (attacks_from_pawn(s, white) & pieces(black, pawn))
    | (attacks_from<knight>(s)      & pieces(knight))
    | (attacks_from_rook(s, occ)     & pieces(rook, queen))
    | (attacks_from_bishop(s, occ)   & pieces(bishop, queen))
    | (attacks_from<king>(s)        & pieces(king));
  }

void position::compute_checkers()
  {
  _checkers = attackers_to(king_square(_side_to_move)) & pieces(~_side_to_move);
  }

/// position::check_blockers() returns a bitboard of all the pieces with color
/// 'c' that are blocking check on the king with color 'kingColor'. A piece
/// blocks a check if removing that piece from the board would result in a
/// position where the king is in check. A check blocking piece can be either a
/// pinned or a discovered check piece, according if its color 'c' is the same
/// or the opposite of 'kingColor'.

bitboard position::_check_blockers(e_color c, e_color king_color) const
  {
  bitboard b, pinners, result = 0;
  e_square ksq = king_square(king_color);

  // Pinners are sliders that give check when a pinned piece is removed
  pinners = ((pieces(rook, queen) & pseudo_attack[rook][ksq])
    | (pieces(bishop, queen) & pseudo_attack[bishop][ksq])) & pieces(~king_color);

  while (pinners)
    {
    b = between[ksq][pop_least_significant_bit(pinners)] & pieces();

    if (!more_than_one(b))
      result |= b & pieces(c);
    }
  return result;
  }

bool position::legal(move m, bitboard pinned) const
  {
  assert(is_ok(m));
  assert(pinned == pinned_pieces(side_to_move()));
  e_color my_color = _side_to_move;
  e_square from = from_square(m);
  assert(color_of(moved_piece(m)) == my_color);
  assert(piece_on(king_square(my_color)) == make_piece(my_color, king));

  // En passant captures are a tricky special case. Because they are rather
  // uncommon, we do it simply by testing whether the king is attacked after
  // the move is made.
  if (type_of(m) == enpassant)
    {
    e_square ksq = king_square(my_color);
    e_square to = to_square(m);
    e_square capsq = to - (my_color == white ? sq_delta_up : sq_delta_down);
    bitboard occ = (pieces() ^ from ^ capsq) | to;

    assert(to == ep_square());
    assert(moved_piece(m) == make_piece(my_color, pawn));
    assert(piece_on(capsq) == make_piece(~my_color, pawn));
    assert(piece_on(to) == no_piece);

    return !(attacks_from_rook(ksq, occ) & pieces(~my_color, queen, rook))
      && !(attacks_from_bishop(ksq, occ) & pieces(~my_color, queen, bishop));
    }

  // If the moving piece is a king, check whether the destination
  // square is attacked by the opponent. Castling moves are checked
  // for legality during move generation.
  if (type_of(piece_on(from)) == king)
    return type_of(m) == castling || !(attackers_to(to_square(m)) & pieces(~my_color));

  // A non-king move is legal if and only if it is not pinned or it
  // is moving along the ray towards or away from the king.
  return   !pinned
    || !(pinned & from)
    || aligned(from, to_square(m), king_square(my_color));
  }

uint64_t position::_compute_hash() const
  {
  uint64_t h = 0;
  for (bitboard b = pieces(); b; )
    {
    e_square s = pop_least_significant_bit(b);
    e_piece pc = piece_on(s);
    h ^= hash_piece[color_of(pc)][type_of(pc)][s];
    }
  if (_side_to_move == black)
    h ^= hash_side;
  if (ep_square() != sq_none)
    h ^= hash_ep[ep_square()];
  //h ^= hash_castle[_castle]; 
  return h;
  }

void position::set_hash()
  {
  hash = _compute_hash();
  }

void position::_compute_lazy_material(int* lazy_piece_v, int* lazy_pawn_v, int* lazy_sq) const
  {
  lazy_piece_v[0] = lazy_piece_v[1] = 0;
  lazy_pawn_v[0] = lazy_pawn_v[1] = 0;
  lazy_sq[0] = lazy_sq[1] = 0;
  for (bitboard b = pieces(); b; )
    {
    e_square s = pop_least_significant_bit(b);
    e_piece pc = piece_on(s);
    e_piecetype pt = type_of(pc);
    e_color c = color_of(pc);
    if (pt == pawn)
      lazy_pawn_v[c] += piece_value[pt];
    else
      lazy_piece_v[c] += piece_value[pt];
    lazy_sq[c] += pcsq[c][pt][s];
    }
  }

void position::set_lazy_material()
  {
  _compute_lazy_material(lazy_piece_value, lazy_pawn_value, lazy_pcsq);
  /*
  lazy_piece_value[0] = lazy_piece_value[1] = 0;
  lazy_pawn_value[0] = lazy_pawn_value[1] = 0;
  lazy_pcsq[0] = lazy_pcsq[1] = 0;
  for (bitboard b = pieces(); b; )
    {
    e_square s = pop_least_significant_bit(b);
    e_piece pc = piece_on(s);
    e_piecetype pt = type_of(pc);
    e_color c = color_of(pc);
    if (pt == pawn)
      lazy_pawn_value[c] += piece_value[pt];
    else
      lazy_piece_value[c] += piece_value[pt];
    lazy_pcsq[c] += pcsq[c][pt][s];
    }
    */
  }

void position::do_castling(e_square from, e_square& to, e_square& rfrom, e_square& rto)
  {
  bool kingside = to > from;
  rfrom = to;
  rto = relative_square(_side_to_move, kingside ? sq_f1 : sq_d1);
  to = relative_square(_side_to_move, kingside ? sq_g1 : sq_c1);

  remove_piece(from, _side_to_move, king);
  remove_piece(rfrom, _side_to_move, rook);
  board[from] = board[rfrom] = no_piece;
  put_piece(to, _side_to_move, king);
  put_piece(rto, _side_to_move, rook);
  }

void position::undo_castling(e_square from, e_square& to, e_square& rfrom, e_square& rto)
  {
  bool kingside = to > from;
  rfrom = to;
  rto = relative_square(_side_to_move, kingside ? sq_f1 : sq_d1);
  to = relative_square(_side_to_move, kingside ? sq_g1 : sq_c1);

  remove_piece(to, _side_to_move, king);
  remove_piece(rto, _side_to_move, rook);
  board[to] = board[rto] = no_piece;
  put_piece(from, _side_to_move, king);
  put_piece(rfrom, _side_to_move, rook);
  }

void position::do_move(move m)
  {
  assert(is_ok(m));
  ++nodes;
  hist_dat[game_ply].m = m;
  hist_dat[game_ply].capture = type_of(m) == enpassant ? piece_on(to_square(m) + pawn_push(~_side_to_move)) : piece_on(to_square(m));
  hist_dat[game_ply].castle = _castle;
  hist_dat[game_ply].ep = ep;
  hist_dat[game_ply].rule50 = rule50;
  hist_dat[game_ply].hash = hash;
  hist_dat[game_ply].checkers = _checkers;
  hist_dat[game_ply].lazy_piece_value[0] = lazy_piece_value[0];
  hist_dat[game_ply].lazy_piece_value[1] = lazy_piece_value[1];
  hist_dat[game_ply].lazy_pawn_value[0] = lazy_pawn_value[0];
  hist_dat[game_ply].lazy_pawn_value[1] = lazy_pawn_value[1];
  hist_dat[game_ply].lazy_pcsq[0] = lazy_pcsq[0];
  hist_dat[game_ply].lazy_pcsq[1] = lazy_pcsq[1];
  ++game_ply;
  ++rule50;
  hash ^= hash_side;

  e_color my_color = _side_to_move;
  e_color other_color = ~my_color;
  e_square from = from_square(m);
  e_square to = to_square(m);
  e_piece pc = piece_on(from);
  e_piecetype pt = type_of(pc);
  e_piecetype captured = type_of(m) == enpassant ? pawn : type_of(piece_on(to));
  assert(pc != no_piece);
  assert(color_of(pc) == my_color);
  assert(piece_on(to) == no_piece || color_of(piece_on(to)) == other_color || type_of(m) == castling);
  assert(captured != king);

  if (type_of(m) == castling)
    {
    e_square rfrom, rto;
    do_castling(from, to, rfrom, rto);
    captured = no_piecetype;
    lazy_pcsq[my_color] += pcsq[my_color][rook][rto] - pcsq[my_color][rook][rfrom];    
    hash ^= hash_piece[my_color][rook][rfrom] ^ hash_piece[my_color][rook][rto];
    }

  if (captured)
    {
    e_square capsq = to;
    if (captured == pawn)
      {
      if (type_of(m) == enpassant)
        {
        capsq += pawn_push(other_color);
        assert(pt == pawn);
        assert(to == ep);
        assert(relative_rank(my_color, to) == rank_6);
        assert(piece_on(to) == no_piece);
        assert(piece_on(capsq) == make_piece(other_color, pawn));

        board[capsq] = no_piece;
        }
      lazy_pawn_value[other_color] -= piece_value[captured];
      }
    else
      lazy_piece_value[other_color] -= piece_value[captured];

    remove_piece(capsq, other_color, captured);
    hash ^= hash_piece[other_color][captured][capsq];
    lazy_pcsq[other_color] -= pcsq[other_color][captured][capsq];
    
    rule50 = 0;
    }

  hash ^= hash_piece[my_color][pt][from] ^ hash_piece[my_color][pt][to];

  if (ep != sq_none)
    {
    hash ^= hash_ep[ep];
    ep = sq_none;
    }

  _castle &= castle_mask[from] & castle_mask[to];

  if (type_of(m) != castling)
    move_piece(from, to, my_color, pt);

  if (pt == pawn)
    {
    // Set en-passant square if the moved pawn can be captured
    if ((int(to) ^ int(from)) == 16
      && (attacks_from_pawn(from + pawn_push(my_color), my_color) & pieces(other_color, pawn)))
      {
      ep = e_square((from + to) / 2);
      hash ^= hash_ep[ep];
      }
    else if (type_of(m) == promotion)
      {
      e_piecetype promotion = promotion_type(m);
      assert(relative_rank(my_color, to) == rank_8);
      assert(promotion >= knight && promotion <= queen);

      remove_piece(to, my_color, pawn);
      put_piece(to, my_color, promotion);

      hash ^= hash_piece[my_color][pawn][to] ^ hash_piece[my_color][promotion][to];

      lazy_pawn_value[my_color] -= piece_value[pawn];
      lazy_piece_value[my_color] += piece_value[promotion];
      lazy_pcsq[my_color] += pcsq[my_color][promotion][to] - pcsq[my_color][pawn][to];

      }
    rule50 = 0;
    }
  lazy_pcsq[my_color] += pcsq[my_color][pt][to] - pcsq[my_color][pt][from];

  _checkers = attackers_to(king_square(other_color)) & pieces(my_color);

  _side_to_move = ~_side_to_move;

  assert((attackers_to(king_square(my_color)) & pieces(other_color)) == 0);

  assert(position_is_ok());
  }

move position::last_move() const
  {
  return game_ply ? hist_dat[game_ply - 1].m : move_none;
  }

void position::undo_move(move m)
  {
  assert(m == hist_dat[game_ply - 1].m);

  assert(is_ok(m));
  _side_to_move = ~_side_to_move;

  e_color my_color = _side_to_move;
  e_square from = from_square(m);
  e_square to = to_square(m);
  e_piecetype pt = type_of(piece_on(to));

  assert(empty(from) || type_of(m) == castling);

  if (type_of(m) == promotion)
    {
    assert(pt == promotion_type(m));
    assert(relative_rank(my_color, to) == rank_8);
    assert(promotion_type(m) >= knight && promotion_type(m) <= queen);

    remove_piece(to, my_color, promotion_type(m));
    put_piece(to, my_color, pawn);
    pt = pawn;
    }
  if (type_of(m) == castling)
    {
    e_square rfrom, rto;
    undo_castling(from, to, rfrom, rto);
    }
  else
    {
    move_piece(to, from, my_color, pt); // Put the piece back at the source square

    if (hist_dat[game_ply - 1].capture)
      {
      e_square capsq = to;

      if (type_of(m) == enpassant)
        {
        capsq -= pawn_push(my_color);

        assert(pt == pawn);
        assert(to == hist_dat[game_ply - 1].ep);
        assert(relative_rank(my_color, to) == rank_6);
        assert(piece_on(capsq) == no_piece);
        }

      put_piece(capsq, ~my_color, type_of(hist_dat[game_ply - 1].capture)); // Restore the captured piece
      }
    }

  --game_ply;

  _castle = hist_dat[game_ply].castle;
  ep = hist_dat[game_ply].ep;
  rule50 = hist_dat[game_ply].rule50;
  hash = hist_dat[game_ply].hash;
  _checkers = hist_dat[game_ply].checkers;

  lazy_piece_value[0] = hist_dat[game_ply].lazy_piece_value[0];
  lazy_piece_value[1] = hist_dat[game_ply].lazy_piece_value[1];
  lazy_pawn_value[0] = hist_dat[game_ply].lazy_pawn_value[0];
  lazy_pawn_value[1] = hist_dat[game_ply].lazy_pawn_value[1];
  lazy_pcsq[0] = hist_dat[game_ply].lazy_pcsq[0];
  lazy_pcsq[1] = hist_dat[game_ply].lazy_pcsq[1];

  assert(position_is_ok());
  }

void position::do_null_move()
  {
  assert(!checkers());
  ++nodes;
  hist_dat[game_ply].m = move_null;
  hist_dat[game_ply].capture = no_piece;
  hist_dat[game_ply].castle = _castle;
  hist_dat[game_ply].ep = ep;
  hist_dat[game_ply].rule50 = rule50;
  hist_dat[game_ply].hash = hash;
  hist_dat[game_ply].checkers = _checkers;
  hist_dat[game_ply].lazy_piece_value[0] = lazy_piece_value[0];
  hist_dat[game_ply].lazy_piece_value[1] = lazy_piece_value[1];
  hist_dat[game_ply].lazy_pawn_value[0] = lazy_pawn_value[0];
  hist_dat[game_ply].lazy_pawn_value[1] = lazy_pawn_value[1];
  hist_dat[game_ply].lazy_pcsq[0] = lazy_pcsq[0];
  hist_dat[game_ply].lazy_pcsq[1] = lazy_pcsq[1];
  if (ep != sq_none)
    {
    hash ^= hash_ep[ep];
    ep = sq_none;
    }
  hash ^= hash_side;
  ++game_ply;
  ++rule50;
  _side_to_move = ~_side_to_move;
  
  assert(position_is_ok());
  }

void position::undo_null_move()
  {
  assert(move_null == hist_dat[game_ply - 1].m);

  _side_to_move = ~_side_to_move;

  --game_ply;

  _castle = hist_dat[game_ply].castle;
  ep = hist_dat[game_ply].ep;
  rule50 = hist_dat[game_ply].rule50;
  hash = hist_dat[game_ply].hash;
  _checkers = hist_dat[game_ply].checkers;

  lazy_piece_value[0] = hist_dat[game_ply].lazy_piece_value[0];
  lazy_piece_value[1] = hist_dat[game_ply].lazy_piece_value[1];
  lazy_pawn_value[0] = hist_dat[game_ply].lazy_pawn_value[0];
  lazy_pawn_value[1] = hist_dat[game_ply].lazy_pawn_value[1];
  lazy_pcsq[0] = hist_dat[game_ply].lazy_pcsq[0];
  lazy_pcsq[1] = hist_dat[game_ply].lazy_pcsq[1];

  assert(position_is_ok());
  }

bool position::position_is_ok() const
  {
  if (_side_to_move != white && _side_to_move != black)
    return false;
  if (piece_on(king_square(white)) != white_king)
    return false;
  if (piece_on(king_square(black)) != black_king)
    return false;
  if (ep_square() != sq_none && relative_rank(_side_to_move, ep_square()) != rank_6)
    return false;

  if (pieces(white) & pieces(black))
    return false;

  if ((pieces(white) | pieces(black)) != pieces())
    return false;

  for (e_piecetype p1 = pawn; p1 <= king; ++p1)
    for (e_piecetype p2 = pawn; p2 <= king; ++p2)
      if (p1 != p2 && (pieces(p1) & pieces(p2)))
        return false;

  if (std::count(board, board + sq_end, white_king) != 1)
    return false;
  if (std::count(board, board + sq_end, black_king) != 1)
    return false;

  if (attackers_to(king_square(~_side_to_move)) & pieces(_side_to_move))
    return false;

  for (e_color c = white; c <= black; ++c)
    for (e_piecetype pt = pawn; pt <= king; ++pt)
      if (piece_count[c][pt] != __popcnt64(pieces(c, pt)))
        return false;

  for (e_color c = white; c <= black; ++c)
    for (e_piecetype pt = pawn; pt <= king; ++pt)
      for (int i = 0; i < piece_count[c][pt]; ++i)
        if (board[piece_list[c][pt][i]] != make_piece(c, pt)
          || index[piece_list[c][pt][i]] != i)
          return false;

  if (_compute_hash() != hash)
    return false;

  int lazy_piece_v[2];
  int lazy_pawn_v[2];
  int lazy_sq[2];
  _compute_lazy_material(lazy_piece_v, lazy_pawn_v, lazy_sq);

  if (lazy_piece_value[0] != lazy_piece_v[0])
    return false;
  if (lazy_piece_value[1] != lazy_piece_v[1])
    return false;
  if (lazy_pawn_value[0] != lazy_pawn_v[0])
    return false;
  if (lazy_pawn_value[1] != lazy_pawn_v[1])
    return false;
  if (lazy_pcsq[0] != lazy_sq[0])
    return false;
  if (lazy_pcsq[1] != lazy_sq[1])
    return false;
  return true;
  }

std::string position::pretty() const
  {
  std::ostringstream ss;

  ss << "\n +---+---+---+---+---+---+---+---+\n";

  for (e_rank r = rank_8; r >= rank_1; --r)
    {
    for (e_file f = file_a; f <= file_h; ++f)
      ss << " | " << piece_to_char[piece_on(make_square(f, r))];

    ss << " |\n +---+---+---+---+---+---+---+---+\n";
    }

  ss << "\nFen: " << fen() << "\nHash: " << std::hex << std::uppercase
    << std::setfill('0') << std::setw(16) << hash << "\nCheckers: ";

  if (checkers() == 0)
    ss << "-";
  for (bitboard b = checkers(); b; )
    ss << to_string(pop_least_significant_bit(b)) << " ";
  ss << "\n";
  return ss.str();
  }

bool position::is_draw() const
  {
  if (rule50 > 99 && (!checkers() || movelist<::legal>(*this).size()))
    return true;

  if (repetitions() >= 2)
    return true;
  return false;
  }

int position::repetitions() const
  {
  int i;
  int r = 0;

  for (i = game_ply - rule50; i < game_ply; ++i)
    if (hist_dat[i].hash == hash)
      ++r;
  return r;
  }