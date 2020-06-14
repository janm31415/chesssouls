#pragma once

#include "types.h"
#include "libchesssouls_api.h"

#include <string>

class position
  {
  public:
    LIB_CHESSSOULS_API position();
    LIB_CHESSSOULS_API position(const std::string& fen);
    LIB_CHESSSOULS_API ~position();

    LIB_CHESSSOULS_API void set_fen(const std::string& fen);
    LIB_CHESSSOULS_API std::string fen() const;

    LIB_CHESSSOULS_API void clear();

    void put_piece(e_square s, e_color c, e_piecetype pt);
    void remove_piece(e_square s, e_color c, e_piecetype pt);
    bool empty(e_square s) const;
    e_piece piece_on(e_square s) const;

    e_color side_to_move() const { return _side_to_move; }

    bitboard pieces() const;
    bitboard pieces(e_color c) const;
    bitboard pieces(e_piecetype pt) const;
    bitboard pieces(e_color c, e_piecetype pt) const;

    e_square ep_square() const { return ep; }

    template <e_piecetype Pt> 
    const e_square* list(e_color c) const;

    template <e_piecetype Pt>
    bitboard attacks_from(e_square s) const;

  private:
    e_piece board[nr_squares];
    bitboard bb_by_type[nr_piecetype];
    bitboard bb_by_color[nr_color];
    int piece_count[nr_color][nr_piecetype];
    e_square piece_list[nr_color][nr_piecetype][16];
    int index[nr_squares];

    e_color _side_to_move;
    uint8_t castle;
    e_square ep;
    int rule50;
    int game_ply;
  }; 

template<e_piecetype Pt>
const e_square* position::list(e_color c) const
  {
  return piece_list[c][Pt];
  }

template <e_piecetype Pt>
bitboard position::attacks_from(e_square s) const
  {
  return  Pt == bishop || Pt == rook ? attacks_bb<Pt>(s, bb_by_type[all_pieces])
    : Pt == queen ? attacks_from<rook>(s) | attacks_from<bishop>(s)
    : step_attacks[Pt][s];
  }