#include "movegen.h"
#include "position.h"
#include "bitboard.h"


namespace
  {

  template <e_movegentype T, e_square delta>
  move* generate_promotions(move* mlist, bitboard pawns_on_7, bitboard target)
    {
    bitboard b = shift_bb<delta>(pawns_on_7) & target;

    while (b)
      {
      e_square to = pop_least_significant_bit(b);
      if (T != quiet)
        {
        *mlist++ = make<promotion>(to - delta, to, queen);
        }
      if (T != captures)
        {
        *mlist++ = make<promotion>(to - delta, to, rook);
        *mlist++ = make<promotion>(to - delta, to, bishop);
        *mlist++ = make<promotion>(to - delta, to, knight);
        }
      }

    return mlist;
    }


  template <e_color my_color, e_movegentype T>
  move* generate_pawn_moves(const position& pos, move* mlist, bitboard target)
    {
    const e_color other_color = (my_color == white ? black : white);
    const e_square up = (my_color == white ? sq_delta_up : sq_delta_down);
    const e_square left = (my_color == white ? sq_delta_up_left : sq_delta_down_right);
    const e_square right = (my_color == white ? sq_delta_up_right : sq_delta_down_left);
    const bitboard rank8bb = (my_color == white ? rank8 : rank1);
    const bitboard rank7bb = (my_color == white ? rank7 : rank2);
    const bitboard rank3bb = (my_color == white ? rank3 : rank6);

    bitboard pawns_on_7 = pos.pieces(my_color, pawn) & rank7bb;
    bitboard pawns_not_on_7 = pos.pieces(my_color, pawn) & ~rank7bb;

    bitboard enemies = T == captures ? target : pos.pieces(other_color);

    bitboard empty_squares, b1, b2;

    // single and double pawn pushes, no promotions
    if (T != captures)
      {
      empty_squares = (T == quiet ? target : ~pos.pieces());
      b1 = shift_bb<up>(pawns_not_on_7) & empty_squares;
      b2 = shift_bb<up>(b1 & rank3bb) & empty_squares;

      while (b1)
        {
        e_square to = pop_least_significant_bit(b1);
        *mlist++ = make_move(to - up, to);
        }

      while (b2)
        {
        e_square to = pop_least_significant_bit(b1);
        *mlist++ = make_move(to - up - up, to);
        }

      }

    // promotions
    if (pawns_on_7 && (target & rank8bb))
      {
      if (T == captures)
        empty_squares = ~pos.pieces();

      mlist = generate_promotions<T, right>(mlist, pawns_on_7, enemies);
      mlist = generate_promotions<T, left>(mlist, pawns_on_7, enemies);
      mlist = generate_promotions<T, up>(mlist, pawns_on_7, empty_squares);
      }

    // standard and en-passant captures
    if (T != quiet)
      {
      b1 = shift_bb<right>(pawns_not_on_7) & enemies;
      b2 = shift_bb<left>(pawns_not_on_7) & enemies;

      while (b1)
        {
        e_square to = pop_least_significant_bit(b1);
        *mlist++ = make_move(to - right, to);
        }

      while (b2)
        {
        e_square to = pop_least_significant_bit(b2);
        *mlist++ = make_move(to - left, to);
        }

      if (pos.ep_square() != sq_none)
        {
        assert(rank_of(pos.ep_square()) == relative_rank(my_color, rank_6));

        b1 = pawns_not_on_7 & attacks_from_pawn(pos.ep_square(), other_color);

        assert(b1);

        while (b1)
          *mlist++ = make<enpassant>(pop_least_significant_bit(b1), pos.ep_square());
        }
      }
    return mlist;
    }

  template <e_piecetype Pt>
  move* generate_moves(const position& pos, move* mlist, e_color my_color, bitboard target)
    {
    assert(Pt != king && Pt != pawn);

    const e_square* it = pos.list<Pt>(my_color);
    for (e_square from = *it; from != sq_none; from = *++it)
      {
      bitboard b = pos.attacks_from<Pt>(from) & target;

      while (b)
        *mlist++ = make_move(from, pop_least_significant_bit(b));
      }

    return mlist;
    }

  template <e_color my_color, e_movegentype T>
  move* generate_all(const position& pos, move* mlist, bitboard target)
    {
    mlist = generate_pawn_moves<my_color, T>(pos, mlist, target);
    mlist = generate_moves<knight>(pos, mlist, my_color, target);
    mlist = generate_moves<bishop>(pos, mlist, my_color, target);
    mlist = generate_moves<rook>(pos, mlist, my_color, target);
    mlist = generate_moves<queen>(pos, mlist, my_color, target);
    return mlist;
    }

  }


template <e_movegentype T>
move* generate(const position& pos, move* mlist)
  {
  e_color my_color = pos.side_to_move();
  bitboard target =
    T == captures ? pos.pieces(~my_color)
    : T == quiet ? ~pos.pieces()
    : T == pseudolegal ? ~pos.pieces(my_color) : 0;

  return my_color == white ?
    generate_all<white, T>(pos, mlist, target)
    : generate_all<black, T>(pos, mlist, target);  
  }





// explicit template instantiation
template move* generate<pseudolegal>(const position& pos, move* mlist);
template move* generate<quiet>(const position& pos, move* mlist);
template move* generate<captures>(const position& pos, move* mlist);