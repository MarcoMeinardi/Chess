#pragma once

#include "./piece.h"

class Board {
private:
    Piece* board[64];
public:
    inline Piece*& operator() (int ind) {
        return board[((ind >> 1) & 0b111000) | (ind & 0b000111)];
    }

    inline Piece*& operator[] (int ind) {
        return board[ind];
    }
};