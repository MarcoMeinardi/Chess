#include "./piece.h"

Piece::Piece (int y_, int x_, int owner_, int type_) {
    has_moved = false;
    y = y_, x = x_;
    owner = owner_;
    type = type_;
    just_moved = false;
}

int Piece::get_type () {
    return type;
}

int Piece::get_owner () {
    return owner;
}

void Piece::move (int y_, int x_) {
    y = y_, x = x_;
}

bool Piece::has_just_moved () {
    return just_moved;
}

void Piece::set_has_just_moved (bool actual) {
    just_moved = actual;
}