#pragma once

#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

#include "./board.h"

#define WHITE 0
#define BLACK 1

#define MAX_DEPTH 3
#define INF 255

const char values[33] = {0, 
	3, 												// Bishop
	0, 0, 											// King
	3, 0, 0, 0, 									// Knight
	1, 0, 0, 0, 0, 0, 0, 0, 						// Pawn
	9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	// Queen
	5												// Rook
};

class Game {
private:
	Board board;
	int turn;
	int last_moved;	// for en passant

	int remaining_pieces[2];
	int moves_without_take_or_pawn_move;

	bool is_checkmate;
	bool is_draw;

	Piece* kings[2];

public:

	Game ();
	~Game ();
	void move_piece (int from, int to);
	int load_moves (int* moves);
	void add_move (int from, int to, int*& moves, Piece* eaten);
	bool is_check (int pos);
	void check_draw ();

	void get_bishop_moves (int pos, int*& moves);
	void get_king_moves (int pos, int*& moves);
	void get_knight_moves (int pos, int*& moves);
	void get_pawn_moves (int pos, int*& moves);
	void get_queen_moves (int pos, int*& moves);
	void get_rook_moves (int pos, int*& moves);

	bool checked_diagonal (int pos);
	bool checked_horizontal (int pos);
	bool checked_by_knight (int pos);


	void print_board ();
	void print_possible_moves(int* moves, int n_moves);
	void human_move (string move);

	int test ();

	int get_best_move ();
	int mini (int depth, int alpha, int beta, int& best_move);
	int maxi (int depth, int alpha, int beta, int& best_move);
	Piece* simulate_move (int move);
	void undo_simulated_move (int move, Piece* eaten);
};

