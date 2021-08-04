#pragma once

#include "./piece.h"

class BitBoard {
private:
    Piece* board[64];
public:
    Piece*& operator() (int ind) {
        return board[((ind >> 1) & 0b111000) | (ind & 0b000111)];
    }

    Piece*& operator[] (int ind) {
        return board[ind];
    }
};