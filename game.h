#pragma once

#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

#include "./board.h"

#define WHITE 0
#define BLACK 1

const char pieces_repr [33] = {'.', 'B', 'K', '.', 'k', '.', '.', '.', 'P', '.', '.', '.', '.', '.', '.', '.', 'Q', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', 'R'};

class Game {
private:
	Board board;
	int turn;
	int last_moved;	// for en passant

	int moves[128];
	int n_moves;

	bool is_checkmate;
	bool is_draw;

	Piece* kings[2];

public:

	Game ();
	~Game ();
	void move_piece (int from, int to);
	void load_moves ();
	void add_move (int from, int to, Piece* eaten);
	bool is_check (int pos);

	void get_bishop_moves (int pos);
	void get_king_moves (int pos);
	void get_knight_moves (int pos);
	void get_pawn_moves (int pos);
	void get_queen_moves (int pos);
	void get_rook_moves (int pos);

	bool checked_by_bishop_queen (int pos);
	bool checked_by_rook_queen (int pos);
	bool checked_by_knight (int pos);
	bool checked_near (int pos);
	bool checked_by_pawn (int pos);

	void print_board ();
	void print_possible_moves();
	void human_move (string move);

	int test ();
};

