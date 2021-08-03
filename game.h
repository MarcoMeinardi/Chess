#pragma once

#include <iostream>

using namespace std;

#include "./piece.h"

const string pieces_repr [6] = {"B", "K", "Kn", "P", "Q", "R"};

class Game {
private:
	Piece* board[8][8];
	int last_moved_y, last_moved_x;	// for en passant
public:

	Game ();
	void print_board ();

	void move_piece (int from_y, int from_x, int to_y, int to_x);
};

