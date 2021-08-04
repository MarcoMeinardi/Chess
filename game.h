#pragma once

#include <iostream>
using namespace std;

#include "./bitboard.h"

#define WHITE 0
#define BLACK 1

const string pieces_repr [33] = {"", "B", "K", "", "Kn", "", "", "", "P", "", "", "", "", "", "", "", "Q", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "R"};

class Game {
private:
	BitBoard board;
	int turn;
	int last_moved;	// for en passant

	int moves[100];
	int n_moves;

public:

	Game ();
	void move_piece (int from, int to);
	void load_moves ();
	void get_bishop_moves (int pos);
	void get_king_moves (int pos);
	void get_knight_moves (int pos);
	void get_pawn_moves (int pos);
	void get_queen_moves (int pos);
	void get_rook_moves (int pos);


	void print_board ();
	void print_possible_moves();
	void human_move (string move);
};

