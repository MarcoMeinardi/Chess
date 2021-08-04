#include "./piece.h"

Piece::Piece (int pos_, int owner_, int type_) {
    has_moved = false;
    pos = pos_;
    owner = owner_;
    type = type_;
    first_double_move = false;
}

int Piece::get_pos () {
    return pos;
}

int Piece::get_type () {
    return type;
}

int Piece::get_owner () {
    return owner;
}

bool Piece::is_first_move () {
    return !has_moved;
}

bool Piece::can_be_en_passant () {
    return first_double_move;
}

void Piece::move (int to) {
    pos = to;
    first_double_move = false;
    if (type == PAWN && to != pos + (1 << 4) && to != pos - (1 << 4)) {
        first_double_move = true;
    }
    has_moved = true;
}