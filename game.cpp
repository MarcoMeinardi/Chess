#include "./game.h"

Game::Game () {
	board[(0 << 4) | (0 & 0xf)] = new Piece	((0 << 4) | (0 & 0xf), WHITE, ROOK);
	board[(0 << 4) | (1 & 0xf)] = new Piece	((0 << 4) | (1 & 0xf), WHITE, KNIGHT);
	board[(0 << 4) | (2 & 0xf)] = new Piece	((0 << 4) | (2 & 0xf), WHITE, BISHOP);
	board[(0 << 4) | (3 & 0xf)] = new Piece	((0 << 4) | (3 & 0xf), WHITE, QUEEN);
	board[(0 << 4) | (4 & 0xf)] = new Piece	((0 << 4) | (4 & 0xf), WHITE, KING);
	board[(0 << 4) | (5 & 0xf)] = new Piece	((0 << 4) | (5 & 0xf), WHITE, BISHOP);
	board[(0 << 4) | (6 & 0xf)] = new Piece	((0 << 4) | (6 & 0xf), WHITE, KNIGHT);
	board[(0 << 4) | (7 & 0xf)] = new Piece	((0 << 4) | (7 & 0xf), WHITE, ROOK);

	for (int i = 0; i < 8; i++) {
		board[(1 << 4) | (i & 0xf)] = new Piece ((1 << 4) | (i & 0xf), WHITE, PAWN);
	}

	for (int i = 2; i < 6; i++) {
		for (int j = 0; j < 8; j++) {
			board[(i << 4) | (j & 0xf)] = nullptr;
		}
	}

	for (int i = 0; i < 8; i++) {
		board[(6 << 4) | (i & 0xf)] = new Piece ((1 << 4) | (i & 0xf), BLACK, PAWN);
	}
    
	board[(7 << 4) | (0 & 0xf)] = new Piece ((7 << 4) | (0 & 0xf), BLACK, ROOK);
	board[(7 << 4) | (1 & 0xf)] = new Piece ((7 << 4) | (1 & 0xf), BLACK, KNIGHT);
	board[(7 << 4) | (2 & 0xf)] = new Piece ((7 << 4) | (2 & 0xf), BLACK, BISHOP);
	board[(7 << 4) | (3 & 0xf)] = new Piece ((7 << 4) | (3 & 0xf), BLACK, QUEEN);
	board[(7 << 4) | (4 & 0xf)] = new Piece ((7 << 4) | (4 & 0xf), BLACK, KING);
	board[(7 << 4) | (5 & 0xf)] = new Piece ((7 << 4) | (5 & 0xf), BLACK, BISHOP);
	board[(7 << 4) | (6 & 0xf)] = new Piece ((7 << 4) | (6 & 0xf), BLACK, KNIGHT);
	board[(7 << 4) | (7 & 0xf)] = new Piece ((7 << 4) | (7 & 0xf), BLACK, ROOK);

	turn = WHITE;
	last_moved = -1;
}

void Game::move_piece (int from, int to) {
	last_moved = to;

	board[from]->move (to);
	board[to] = board[from];
	board[from] = nullptr;
}


void Game::print_board () {
	for (int i = 7; i >= 0; i--) {
		cout << "\t" << (i + 1) << "  |";
		for (int j = 0; j < 8; j++) {
			cout << "\t";
			if (board[(i << 4) | (j & 0xf)]) {
				cout << pieces_repr[board[(i << 4) | (j & 0xf)]->get_type ()];
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