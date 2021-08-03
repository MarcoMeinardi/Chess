#pragma once

#include <iostream>
using namespace std;

#include "./piece.h"

#define BISHOP	0
#define KING	1
#define KNIGHT	2
#define PAWN	3
#define QUEEN	4
#define ROOK	5

#define WHITE 0
#define BLACK 1

const string pieces_repr [6] = {"B", "K", "Kn", "P", "Q", "R"};

class Game {
private:
	Piece* board[64];
	int last_moved;	// for en passant
public:

	Game ();
	void move_piece (int from, int to);

	void print_board ();
};

