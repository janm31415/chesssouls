#pragma once

#include "types.h"
#include "bitboard.h"
#include "libchesssouls_api.h"

#include <string>

struct hist_t
  {
  move m;
  uint8_t castle;
  e_square ep;
  int rule50;
  uint64_t hash;
  e_piece capture;  
  bitboard checkers;
  int lazy_piece_value[2];
  int lazy_pawn_value[2];
  int lazy_pcsq[2];
  };

class position
  {
  public:
    LIB_CHESSSOULS_API position();
    LIB_CHESSSOULS_API position(const std::string& fen);
    LIB_CHESSSOULS_API ~position();

    LIB_CHESSSOULS_API void set_fen(const std::string& fen);
    LIB_CHESSSOULS_API std::string fen() const;

    LIB_CHESSSOULS_API void clear();

    LIB_CHESSSOULS_API bool position_is_ok() const;

    LIB_CHESSSOULS_API std::string pretty() const;

    LIB_CHESSSOULS_API int repetitions() const;
    LIB_CHESSSOULS_API bool is_draw() const;

    void put_piece(e_square s, e_color c, e_piecetype pt);
    void move_piece(e_square from, e_square to, e_color c, e_piecetype pt);
    void remove_piece(e_square s, e_color c, e_piecetype pt);
    bool empty(e_square s) const;
    e_piece piece_on(e_square s) const;
    e_piece moved_piece(move m) const;
    bool capture(move m) const;
    bool capture_or_promotion(move m) const;
    bool endgame() const;

    LIB_CHESSSOULS_API void set_side_to_move(e_color c)
      {
      _side_to_move = c;
      }

    e_color side_to_move() const { return _side_to_move; }

    bitboard pieces() const;
    bitboard pieces(e_color c) const;
    bitboard pieces(e_piecetype pt) const;
    bitboard pieces(e_piecetype pt1, e_piecetype pt2) const;
    bitboard pieces(e_color c, e_piecetype pt) const;
    bitboard pieces(e_color c, e_piecetype pt1, e_piecetype pt2) const;

    e_square ep_square() const { return ep; }

    int ply() const { return game_ply; }

    move get_previously_played_move(int ply) const
      {
      return hist_dat[ply].m;
      }

    template <e_piecetype Pt> 
    const e_square* list(e_color c) const;

    template <e_piecetype Pt>
    bitboard attacks_from(e_square s) const;

    bitboard attackers_to(e_square s) const;
    bitboard attackers_to(e_square s, bitboard occ) const;

    e_square king_square(e_color c) const 
      {
      return piece_list[c][king][0];
      }

    int can_castle(e_color c) const
      {
      /*
      color = 0 => castle & 1 or castle & 2  => castle & 3
      color = 1 => castle & 4 or castle & 8  => castle & 12
      */
      return _castle & (3 << (c * 2));
      }

    int can_castle_kingside(e_color c) const
      {
      return _castle & (1 << (c * 2));
      }

    int can_castle_queenside(e_color c) const
      {
      return _castle & (2 << (c * 2));
      }

    bool castling_path_obstructed(int castle) const
      {
      return bb_by_type[all_pieces] & castling_path[castle];
      }

    void set_hash();
    void set_lazy_material();

    void compute_checkers();

    bitboard checkers() const
      {
      return _checkers;
      }

    bitboard pinned_pieces(e_color c) const
      {
      return _check_blockers(c, c);
      }

    bool legal(move m, bitboard pinned) const;

    int fifty() const 
      { 
      return rule50; 
      }

    int non_pawn_material_value(e_color c) const
      {
      return lazy_piece_value[c];
      }

    int pawn_material_value(e_color c) const
      {
      return lazy_pawn_value[c];
      }

    int positional_value(e_color c) const
      {
      return lazy_pcsq[c];
      }

    uint64_t zobrist_key() const
      {
      return hash;
      }

    uint64_t compute_evaluation_zobrist_key() const;

    int move_ordering_score(move m) const;

    LIB_CHESSSOULS_API void do_move(move m);
    LIB_CHESSSOULS_API move last_move() const;
    LIB_CHESSSOULS_API void undo_move(move m);
    void do_null_move();
    void undo_null_move();

    int see(move m, int asymmThreshold) const;

  private:
    bitboard _check_blockers(e_color c, e_color king_color) const;
    void do_castling(e_square from, e_square& to, e_square& rfrom, e_square& rto);
    void undo_castling(e_square from, e_square& to, e_square& rfrom, e_square& rto);
    uint64_t _compute_hash() const;
    void _compute_lazy_material(int* lazy_piece_v, int* lazy_pawn_v, int* lazy_sq) const;

  private:
    e_piece board[nr_squares];
    bitboard bb_by_type[nr_piecetype];
    bitboard bb_by_color[nr_color];
    bitboard _checkers;
    int piece_count[nr_color][nr_piecetype];
    e_square piece_list[nr_color][nr_piecetype][16];
    int index[nr_squares];

    e_color _side_to_move;
    uint8_t _castle;
    e_square ep;
    int rule50;
    int game_ply;
    uint64_t hash;
    hist_t hist_dat[5899];
    uint64_t nodes;
    int lazy_piece_value[2];
    int lazy_pawn_value[2];
    int lazy_pcsq[2];
  }; 

template<e_piecetype Pt>
const e_square* position::list(e_color c) const
  {
  return piece_list[c][Pt];
  }

template <e_piecetype Pt>
inline bitboard position::attacks_from(e_square s) const
  {
  return step_attacks[Pt][s];
  }

template <>
inline bitboard position::attacks_from<bishop>(e_square s) const
  {
  return attacks_from_bishop(s, bb_by_type[all_pieces]);
  }

template <>
inline bitboard position::attacks_from<rook>(e_square s) const
  {
  return attacks_from_rook(s, bb_by_type[all_pieces]);
  }

template <>
inline bitboard position::attacks_from<queen>(e_square s) const
  {
  return attacks_from_bishop(s, bb_by_type[all_pieces]) | attacks_from_rook(s, bb_by_type[all_pieces]);
  }