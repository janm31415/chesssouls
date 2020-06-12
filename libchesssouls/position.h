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

  private:
    e_piece board[nr_squares];
    bitboard bb_by_type[nr_piecetype];
    bitboard bb_by_color[nr_color];

    e_color side_to_move;
    uint8_t castle;
    e_square ep;
    int rule50;
    int game_ply;
  }; 