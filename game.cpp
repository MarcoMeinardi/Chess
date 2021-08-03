#include "./game.h"

Game::Game () {
	board[0][0] = new Piece	(0, 0, WHITE, ROOK);
	board[0][1] = new Piece	(0, 1, WHITE, KNIGHT);
	board[0][2] = new Piece	(0, 2, WHITE, BISHOP);
	board[0][3] = new Piece	(0, 3, WHITE, QUEEN);
	board[0][4] = new Piece	(0, 4, WHITE, KING);
	board[0][5] = new Piece	(0, 5, WHITE, BISHOP);
	board[0][6] = new Piece	(0, 6, WHITE, KNIGHT);
	board[0][7] = new Piece	(0, 7, WHITE, ROOK);

	for (int i = 0; i < 8; i++) {
		board[1][i] = new Piece (1, i, WHITE, PAWN);
	}

	for (int i = 2; i < 6; i++) {
		for (int j = 0; j < 8; j++) {
			board[i][j] = nullptr;
		}
	}

	for (int i = 0; i < 8; i++) {
		board[6][i] = new Piece (6, i, BLACK, PAWN);
	}
    
	board[7][0] = new Piece	(7, 0, BLACK, ROOK);
	board[7][1] = new Piece	(7, 1, BLACK, KNIGHT);
	board[7][2] = new Piece	(7, 2, BLACK, BISHOP);
	board[7][3] = new Piece	(7, 3, BLACK, QUEEN);
	board[7][4] = new Piece	(7, 4, BLACK, KING);
	board[7][5] = new Piece	(7, 5, BLACK, BISHOP);
	board[7][6] = new Piece	(7, 6, BLACK, KNIGHT);
	board[7][7] = new Piece	(7, 7, BLACK, ROOK);

	last_moved_y = -1, last_moved_x = -1;
}

void Game::print_board () {
	for (int i = 0; i < 8; i++) {
		cout << "\t" << (8 - i) << "  |";
		for (int j = 0; j < 8; j++) {
			cout << "\t";
			if (board[7 - i][j]) {
				cout << pieces_repr[board[7 - i][j]->get_type ()];
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

void Game::move_piece (int from_y, int from_x, int to_y, int to_x) {
	if (last_moved_y != -1) {
		board[last_moved_y][last_moved_x]->set_has_just_moved (false);
	}
	board[from_y][from_x]->set_has_just_moved (true);
	last_moved_y = to_y;
	last_moved_x = to_x;

	board[from_y][from_x]->move (to_y, to_x);
	board[to_y][to_x] = board[from_y][from_x];
	board[from_y][from_x] = nullptr;
}