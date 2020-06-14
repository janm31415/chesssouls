#include "movegen.h"
#include "position.h"
#include "bitboard.h"


namespace
  {

  move* generate_castling_kingside(const position& pos, move* mlist, e_color my_color)
    {
    assert(!pos.checkers());
    if (!pos.can_castle_kingside(my_color))
      return mlist;
    if (pos.castling_path_obstructed(1 << (my_color * 2)))
      return mlist;
    e_square kfrom = pos.king_square(my_color);
    e_square kto = relative_square(my_color, sq_g1);
    bitboard enemies = pos.pieces(~my_color);
    for (e_square s = kto; s != kfrom; s += sq_delta_left)
      {
      if (pos.attackers_to(s) & enemies)
        return mlist;
      }

    *mlist++ = make<castling>(kfrom, kto);
    return mlist;
    }

  move* generate_castling_queenside(const position& pos, move* mlist, e_color my_color)
    {
    assert(!pos.checkers());
    if (!pos.can_castle_queenside(my_color))
      return mlist;
    if (pos.castling_path_obstructed(2 << (my_color * 2)))
      return mlist;
    e_square kfrom = pos.king_square(my_color);
    e_square kto = relative_square(my_color, sq_c1);
    bitboard enemies = pos.pieces(~my_color);
    for (e_square s = kto; s != kfrom; s += sq_delta_right)
      {
      if (pos.attackers_to(s) & enemies)
        return mlist;
      }

    *mlist++ = make<castling>(kfrom, kto);
    return mlist;
    }

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

    bitboard enemies =
      T == evasions ? pos.pieces(other_color) & target :
      T == captures ? target : pos.pieces(other_color);

    bitboard empty_squares, b1, b2;

    // single and double pawn pushes, no promotions
    if (T != captures)
      {
      empty_squares = (T == quiet ? target : ~pos.pieces());
      b1 = shift_bb<up>(pawns_not_on_7) & empty_squares;
      b2 = shift_bb<up>(b1 & rank3bb) & empty_squares;

      if (T == evasions) // Consider only blocking squares
        {
        b1 &= target;
        b2 &= target;
        }

      while (b1)
        {
        e_square to = pop_least_significant_bit(b1);
        *mlist++ = make_move(to - up, to);
        }

      while (b2)
        {
        e_square to = pop_least_significant_bit(b2);
        *mlist++ = make_move(to - up - up, to);
        }

      }

    // promotions
    if (pawns_on_7 && (T != evasions || (target & rank8bb)))
      {
      if (T == captures)
        empty_squares = ~pos.pieces();

      if (T == evasions)
        empty_squares &= target;

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

        // An en passant capture can be an evasion only if the checking piece
        // is the double pushed pawn and so is in the target. Otherwise this
        // is a discovery check and we are forced to do otherwise.
        if (T == evasions && !(target & (pos.ep_square() - up)))
          return mlist;

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

    if (T != evasions)
      {
      e_square ksq = pos.king_square(my_color);
      bitboard b = pos.attacks_from<king>(ksq) & target;
      while (b)
        *mlist++ = make_move(ksq, pop_least_significant_bit(b));
      }
    if (T != evasions && T != captures)
      {
      if (pos.can_castle(my_color))
        {
        mlist = generate_castling_kingside(pos, mlist, my_color);
        mlist = generate_castling_queenside(pos, mlist, my_color);
        }
      }
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
    : T == non_evasions ? ~pos.pieces(my_color) : 0;

  return my_color == white ?
    generate_all<white, T>(pos, mlist, target)
    : generate_all<black, T>(pos, mlist, target);
  }

/*
We are in check. Generate all pseudo-legal check evasions.
*/
template <>
move* generate<evasions>(const position& pos, move* mlist)
  {
  assert(pos.checkers());

  e_color my_color = pos.side_to_move();
  e_square ksq = pos.king_square(my_color);
  bitboard slider_attacks = 0;
  bitboard sliders = pos.checkers() & ~pos.pieces(knight, pawn);
  /*
  sliders are queen, rooks, or bishops that checked us.
  All squares from the king to this slider cannot be used.
  */
  while (sliders)
    {
    e_square square_in_check = pop_least_significant_bit(sliders);
    slider_attacks |= line[square_in_check][ksq] ^ square_in_check;
    }

  bitboard b = pos.attacks_from<king>(ksq) & ~pos.pieces(my_color) & ~slider_attacks;
  while (b)
    *mlist++ = make_move(ksq, pop_least_significant_bit(b));

  if (more_than_one(pos.checkers()))
    return mlist; // double check, only a king move can save the day

  // Generate blocking evasions or captures of the checking piece
  e_square checksq = least_significant_bit(pos.checkers());
  bitboard target = between[checksq][ksq] | checksq;
  return my_color == white ?
    generate_all<white, evasions>(pos, mlist, target)
    : generate_all<black, evasions>(pos, mlist, target);
  }

template <>
move* generate<legal>(const position& pos, move* mlist)
  {  
  move* current = mlist;

  move* end = pos.checkers() ? generate<evasions>(pos, mlist)
    : generate<non_evasions>(pos, mlist);

  e_square ksq = pos.king_square(pos.side_to_move());
  bitboard pinned = pos.pinned_pieces(pos.side_to_move());

  while (current != end)
    {
    if ((pinned || from_square(*current) == ksq || type_of(*current) == enpassant)
      && !pos.legal(*current, pinned))
      *current = *(--end);
    else
      ++current;
    }
  return end;
  }

// explicit template instantiation
template move* generate<non_evasions>(const position& pos, move* mlist);
template move* generate<quiet>(const position& pos, move* mlist);
template move* generate<captures>(const position& pos, move* mlist);