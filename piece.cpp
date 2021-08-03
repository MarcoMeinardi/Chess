#include "./piece.h"

Piece::Piece (int pos_, int owner_, int type_) {
    has_moved = false;
    pos = pos_;
    owner = owner_;
    type = type_;
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

void Piece::move (int to) {
    pos = to;
    has_moved = true;
}