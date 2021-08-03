#include "./game.h"

Game::Game () {
	board[0 | (0 << 3)] = new Piece	(0 | (0 << 3), WHITE, ROOK);
	board[0 | (1 << 3)] = new Piece	(0 | (1 << 3), WHITE, KNIGHT);
	board[0 | (2 << 3)] = new Piece	(0 | (2 << 3), WHITE, BISHOP);
	board[0 | (3 << 3)] = new Piece	(0 | (3 << 3), WHITE, QUEEN);
	board[0 | (4 << 3)] = new Piece	(0 | (4 << 3), WHITE, KING);
	board[0 | (5 << 3)] = new Piece	(0 | (5 << 3), WHITE, BISHOP);
	board[0 | (6 << 3)] = new Piece	(0 | (6 << 3), WHITE, KNIGHT);
	board[0 | (7 << 3)] = new Piece	(0 | (7 << 3), WHITE, ROOK);

	for (int i = 0; i < 8; i++) {
		board[1 | (i << 3)] = new Piece (1 | (i << 3), WHITE, PAWN);
	}

	for (int i = 2; i < 6; i++) {
		for (int j = 0; j < 8; j++) {
			board[i | (j << 3)] = nullptr;
		}
	}

	for (int i = 0; i < 8; i++) {
		board[6 | (i << 3)] = new Piece (6 | (i << 3), BLACK, PAWN);
	}
    
	board[7 | (0 << 3)] = new Piece (7 | (0 << 3), BLACK, ROOK);
	board[7 | (1 << 3)] = new Piece (7 | (1 << 3), BLACK, KNIGHT);
	board[7 | (2 << 3)] = new Piece (7 | (2 << 3), BLACK, BISHOP);
	board[7 | (3 << 3)] = new Piece (7 | (3 << 3), BLACK, QUEEN);
	board[7 | (4 << 3)] = new Piece (7 | (4 << 3), BLACK, KING);
	board[7 | (5 << 3)] = new Piece (7 | (5 << 3), BLACK, BISHOP);
	board[7 | (6 << 3)] = new Piece (7 | (6 << 3), BLACK, KNIGHT);
	board[7 | (7 << 3)] = new Piece (7 | (7 << 3), BLACK, ROOK);

	last_moved = -1;
}

void Game::move_piece (int from, int to) {
	last_moved = to;

	board[from]->move (to);
	board[to] = board[from];
	board[from] = nullptr;
}

void Game::print_board () {
	for (int i = 0; i < 8; i++) {
		cout << "\t" << (8 - i) << "  |";
		for (int j = 0; j < 8; j++) {
			cout << "\t";
			if (board[(7 - i) | (j << 3)]) {
				cout << pieces_repr[board[(7 - i) | (j << 3)]->get_type ()];
			}
		}
		if (i != 7) {
			cout << endl << "\t   |" << endl;
		} else {
			cout << endl;
		}
	}

	cout << "\t\t";
	for (int j = 0; j < 7; j++) {
		cout << "________";
	}
	cout << "_" << endl << endl;

	cout << "\t";
	for (int j = 0; j < 8; j++) {
		cout << "\t" << (char)(j + 'a');
	}
	cout << endl << endl << endl;
}