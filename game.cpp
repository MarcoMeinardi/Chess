#include "./game.h"

Game::Game () {
	board[(0 << 3) | (0 & 0x7)] = new Piece	((0 << 4) | (0 & 0xf), WHITE, ROOK);
	board[(0 << 3) | (1 & 0x7)] = new Piece	((0 << 4) | (1 & 0xf), WHITE, KNIGHT);
	board[(0 << 3) | (2 & 0x7)] = new Piece	((0 << 4) | (2 & 0xf), WHITE, BISHOP);
	board[(0 << 3) | (3 & 0x7)] = new Piece	((0 << 4) | (3 & 0xf), WHITE, QUEEN);
	board[(0 << 3) | (4 & 0x7)] = new Piece	((0 << 4) | (4 & 0xf), WHITE, KING);
	board[(0 << 3) | (5 & 0x7)] = new Piece	((0 << 4) | (5 & 0xf), WHITE, BISHOP);
	board[(0 << 3) | (6 & 0x7)] = new Piece	((0 << 4) | (6 & 0xf), WHITE, KNIGHT);
	board[(0 << 3) | (7 & 0x7)] = new Piece	((0 << 4) | (7 & 0xf), WHITE, ROOK);

	for (int i = 0; i < 8; i++) {
		board[(1 << 3) | (i & 0x7)] = new Piece ((1 << 4) | (i & 0xf), WHITE, PAWN);
	}

	for (int i = 2; i < 6; i++) {
		for (int j = 0; j < 8; j++) {
			board[(i << 3) | (j & 0x7)] = nullptr;
		}
	}

	for (int i = 0; i < 8; i++) {
		board[(6 << 3) | (i & 0x7)] = new Piece ((1 << 4) | (i & 0xf), BLACK, PAWN);
	}
    
	board[(7 << 3) | (0 & 0x7)] = new Piece ((7 << 4) | (0 & 0xf), BLACK, ROOK);
	board[(7 << 3) | (1 & 0x7)] = new Piece ((7 << 4) | (1 & 0xf), BLACK, KNIGHT);
	board[(7 << 3) | (2 & 0x7)] = new Piece ((7 << 4) | (2 & 0xf), BLACK, BISHOP);
	board[(7 << 3) | (3 & 0x7)] = new Piece ((7 << 4) | (3 & 0xf), BLACK, QUEEN);
	board[(7 << 3) | (4 & 0x7)] = new Piece ((7 << 4) | (4 & 0xf), BLACK, KING);
	board[(7 << 3) | (5 & 0x7)] = new Piece ((7 << 4) | (5 & 0xf), BLACK, BISHOP);
	board[(7 << 3) | (6 & 0x7)] = new Piece ((7 << 4) | (6 & 0xf), BLACK, KNIGHT);
	board[(7 << 3) | (7 & 0x7)] = new Piece ((7 << 4) | (7 & 0xf), BLACK, ROOK);

	turn = WHITE;
	last_moved = -1;
}

void Game::move_piece (int from, int to) {
	last_moved = to;

	if (board(from)->get_type () == KING) {
		// short-castle
		if (to == from + 2) {
			board(from)->move (to);
			board(to) = board(from);
			board(from) = nullptr;
			board(from + 3)->move (from + 1);
			board(from + 1) = board(from + 3);
			board(from + 3) = nullptr;
			return;
		} else if (to == from - 2) {
			board(from)->move (to);
			board(to) = board(from);
			board(from) = nullptr;
			board(from - 4)->move (from - 1);
			board(from - 1) = board(from - 4);
			board(from - 4) = nullptr;
			return;
		}
	}
	board(from)->move (to);
	board(to) = board(from);
	board(from) = nullptr;
}

void Game::load_moves () {
	n_moves = 0;
	
	for (int i = 0; i < 64; i++) {	// y = i >> 3, x = i & 0x7
		if (board[i] && board[i]->get_owner () == turn) {
			switch (board[i]->get_type ()) {
			case BISHOP:
				get_bishop_moves (((i << 1) & 0b1110000) | (i & 0b111));
				break;
			case KING:
				get_king_moves (((i << 1) & 0b1110000) | (i & 0b111));
				break;
			case KNIGHT:
				get_knight_moves (((i << 1) & 0b1110000) | (i & 0b111));
				break;
			case PAWN:
				get_pawn_moves (((i << 1) & 0b1110000) | (i & 0b111));
				break;
			case QUEEN:
				get_queen_moves (((i << 1) & 0b1110000) | (i & 0b111));
				break;
			case ROOK:
				get_rook_moves (((i << 1) & 0b1110000) | (i & 0b111));
				break;
			}
		}
	}
}

void Game::get_bishop_moves (int pos) {
	// up-rigth
	int p;
	p = pos + ((1 << 4) | (1 & 0xf));
	while (!(p & 0b10001000)) {
		if (board(p)) {
			if (board(p)->get_owner () != turn) {
				moves[n_moves++] = pos | (p << 8);
			}
			break;
		}
		moves[n_moves++] = pos | (p << 8);
		p += (1 << 4) | (1 & 0xf);
	}

	// down-right
	p = pos + (-(1 << 4) | (1 & 0xf));
	while (!(p & 0b10001000)) {
		if (board(p)) {
			if (board(p)->get_owner () != turn) {
				moves[n_moves++] = pos | (p << 8);
			}
			break;
		}
		moves[n_moves++] = pos | (p << 8);
		p += -(1 << 4) | (1 & 0xf);
	}

	// down-left
	p = pos + (-(1 << 4) - (1 & 0xf));
	while (!(p & 0b10001000)) {
		if (board(p)) {
			if (board(p)->get_owner () != turn) {
				moves[n_moves++] = pos | (p << 8);
			}
			break;
		}
		moves[n_moves++] = pos | (p << 8);
		p += -(1 << 4) - (1 & 0xf);
	}

	// up-left
	p = pos + ((1 << 4) - (1 & 0xf));
	while (!(p & 0b10001000)) {
		if (board(p)) {
			if (board(p)->get_owner () != turn) {
				moves[n_moves++] = pos | (p << 8);
			}
			break;
		}
		moves[n_moves++] = pos | (p << 8);
		p += (1 << 4) - (1 & 0xf);
	}
}
void Game::get_king_moves (int pos) {
	int p;
	// up
	p = pos + (1 << 4);
	if (!(p & 0b10001000) && (!board(p) || (board(p)->get_owner () != turn))) {
		moves[n_moves++] = pos | (p << 8);
	}

	// up-right
	p = pos + ((1 << 4) | (1 & 0xf));
	if (!(p & 0b10001000) && (!board(p) || (board(p)->get_owner () != turn))) {
		moves[n_moves++] = pos | (p << 8);
	}

	// right
	p = pos + 1;
	if (!(p & 0b10001000) && (!board(p) || (board(p)->get_owner () != turn))) {
		moves[n_moves++] = pos | (p << 8);
	}

	// down-right
	p = pos + (-(1 << 4) | (1 & 0xf));
	if (!(p & 0b10001000) && (!board(p) || (board(p)->get_owner () != turn))) {
		moves[n_moves++] = pos | (p << 8);
	}

	// down
	p = pos - (1 << 4);
	if (!(p & 0b10001000) && (!board(p) || (board(p)->get_owner () != turn))) {
		moves[n_moves++] = pos | (p << 8);
	}

	// down-left
	p = pos + (-(1 << 4) - (1 & 0xf));
	if (!(p & 0b10001000) && (!board(p) || (board(p)->get_owner () != turn))) {
		moves[n_moves++] = pos | (p << 8);
	}

	// left
	p = pos - 1;
	if (!(p & 0b10001000) && (!board(p) || (board(p)->get_owner () != turn))) {
		moves[n_moves++] = pos | (p << 8);
	}

	// up-left
	p = pos + ((1 << 4) - (1 & 0xf));
	if (!(p & 0b10001000) && (!board(p) || (board(p)->get_owner () != turn))) {
		moves[n_moves++] = pos | (p << 8);
	}

	// short-castle
	if (
	board(pos)->is_first_move () && 
	!board(pos + 1) && !board(pos + 2) && board(pos + 3) && 
	board(pos + 3)->get_type () == ROOK && board(pos + 3)->is_first_move()) {
		moves[n_moves++] = pos | ((pos + 2) << 8);
	}
	// long-castle
	if (
	board(pos)->is_first_move () && 
	!board(pos - 1) && !board(pos - 2) && !board(pos - 3) && board(pos - 4) &&
	board(pos - 4)->get_type () == ROOK && board(pos - 4)->is_first_move()) {
		moves[n_moves++] = pos | ((pos - 2) << 8);
	}
}
void Game::get_knight_moves (int pos) {
	int p;
	// up-right
	p = pos + ((2 << 4) | (1 & 0xf));
	if (!(p & 0b10001000) && (!board(p) || (board(p)->get_owner () != turn))) {
		moves[n_moves++] = pos | (p << 8);
	}

	// right-up
	p = pos + ((1 << 4) | (2 & 0xf));
	if (!(p & 0b10001000) && (!board(p) || (board(p)->get_owner () != turn))) {
		moves[n_moves++] = pos | (p << 8);
	}

	// right-down
	p = pos + (-(1 << 4) | (2 & 0xf));
	if (!(p & 0b10001000) && (!board(p) || (board(p)->get_owner () != turn))) {
		moves[n_moves++] = pos | (p << 8);
	}

	// down-right
	p = pos + (-(2 << 4) | (1 & 0xf));
	if (!(p & 0b10001000) && (!board(p) || (board(p)->get_owner () != turn))) {
		moves[n_moves++] = pos | (p << 8);
	}

	// down-left
	p = pos + (-(2 << 4) - (1 & 0xf));
	if (!(p & 0b10001000) && (!board(p) || (board(p)->get_owner () != turn))) {
		moves[n_moves++] = pos | (p << 8);
	}

	// left-down
	p = pos + (-(1 << 4) - (2 & 0xf));
	if (!(p & 0b10001000) && (!board(p) || (board(p)->get_owner () != turn))) {
		moves[n_moves++] = pos | (p << 8);
	}

	// left-up
	p = pos + ((1 << 4) - (2 & 0xf));
	if (!(p & 0b10001000) && (!board(p) || (board(p)->get_owner () != turn))) {
		moves[n_moves++] = pos | (p << 8);
	}

	// up-left
	p = pos + ((2 << 4) - (1 & 0xf));
	if (!(p & 0b10001000) && (!board(p) || (board(p)->get_owner () != turn))) {
		moves[n_moves++] = pos | (p << 8);
	}
}
void Game::get_pawn_moves (int pos) {
	if (turn == WHITE) {
		int p;
		p = pos + (1 << 4);
		if (!(p & 0b10001000) && !board(p)) {
			moves[n_moves++] = pos | (p << 8);
			p += (1 << 4);
			if (board(pos)->is_first_move () && (!(p & 0b10001000) && !board(p))) {
				moves[n_moves++] = pos | (p << 8);
			}
		}

		// take
		p = pos + ((1 << 4) | (1 & 0xf));
		if (!(p & 0b10001000) && board(p) && board(p)->get_owner () == BLACK) {
			moves[n_moves++] = pos | (p << 8);
		}
		p = pos + ((1 << 4) - (1 & 0xf));
		if (!(p & 0b10001000) && board(p) && board(p)->get_owner () == BLACK) {
			moves[n_moves++] = pos | (p << 8);
		}

		// en passant
		if (last_moved == pos + 1) {
			if (board(last_moved)->can_be_en_passant ()) {
				moves[n_moves++] = pos | ((pos + ((1 << 4) | 1)) << 8);
			}
		} else if (last_moved == pos - 1) {
			if (board(last_moved)->can_be_en_passant ()) {
				moves[n_moves++] = pos | ((pos + ((1 << 4) - 1)) << 8);
			}
		}
	} else {
		int p;
		p = pos - (1 << 4);
		if (!(p & 0b10001000) && !board(p)) {
			moves[n_moves++] = pos | (p << 8);
			p -= (1 << 4);
			if (board(pos)->is_first_move () && (!(p & 0b10001000) && !board(p))) {
				moves[n_moves++] = pos | (p << 8);
			}
		}

		// take
		p = pos + (-(1 << 4) | (1 & 0xf));
		if (!(p & 0b10001000) && board(p) && board(p)->get_owner () == BLACK) {
			moves[n_moves++] = pos | (p << 8);
		}
		p = pos + (-(1 << 4) - (1 & 0xf));
		if (!(p & 0b10001000) && board(p) && board(p)->get_owner () == BLACK) {
			moves[n_moves++] = pos | (p << 8);
		}

		// en passant
		if (last_moved == pos + 1) {
			if (board(last_moved)->can_be_en_passant ()) {
				moves[n_moves++] = pos | ((pos + (-(1 << 4) | 1)) << 8);
			}
		} else if (last_moved == pos - 1) {
			if (board(last_moved)->can_be_en_passant ()) {
				moves[n_moves++] = pos | ((pos + (-(1 << 4) - 1)) << 8);
			}
		}
	}
}
void Game::get_queen_moves (int pos) {
	get_bishop_moves (pos);
	get_rook_moves (pos);
}
void Game::get_rook_moves (int pos) {
	// up
	int p;
	p = pos + (1 << 4);
	while (!(p & 0b10001000)) {
		if (board(p)) {
			if (board(p)->get_owner () != turn) {
				moves[n_moves++] = pos | (p << 8);
			}
			break;
		}
		moves[n_moves++] = pos | (p << 8);
		p += (1 << 4);
	}

	// right
	p = pos + (1 & 0xf);
	while (!(p & 0b10001000)) {
		if (board(p)) {
			if (board(p)->get_owner () != turn) {
				moves[n_moves++] = pos | (p << 8);
			}
			break;
		}
		moves[n_moves++] = pos | (p << 8);
		p += (1 & 0xf);
	}

	// down
	p = pos - (1 << 4);
	while (!(p & 0b10001000)) {
		if (board(p)) {
			if (board(p)->get_owner () != turn) {
				moves[n_moves++] = pos | (p << 8);
			}
			break;
		}
		moves[n_moves++] = pos | (p << 8);
		p -= (1 << 4);
	}

	// left
	p = pos - (1 & 0xf);
	while (!(p & 0b10001000)) {
		if (board(p)) {
			if (board(p)->get_owner () != turn) {
				moves[n_moves++] = pos | (p << 8);
			}
			break;
		}
		moves[n_moves++] = pos | (p << 8);
		p -= (1 & 0xf);
	}
}


void Game::print_board () {
	for (int i = 7; i >= 0; i--) {
		cout << "\t" << (i + 1) << "  |";
		for (int j = 0; j < 8; j++) {
			cout << "\t";
			if (board[(i << 3) | (j & 0x7)]) {
				cout << pieces_repr[board[(i << 3) | (j & 0x7)]->get_type ()];
			}
		}
		if (i != 0) {
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

void Game::print_possible_moves () {
	load_moves ();
	for (int i = 0; i < n_moves; i++) {
		int move = moves[i];
		int from = move & 0xff;
		int to = move >> 8;
		cout << pieces_repr[board(from)->get_type ()] << "\t";
		cout << (char)((from & 0xf) + 'a') << " " << ((from >> 4) + 1) << "\t->\t";
		cout << (char)((to & 0xf) + 'a') << " " << ((to >> 4) + 1) << endl;
	}
}

void Game::human_move (string move) {	// "a1 h8"
	int from_col = move[0] - 'a';
	int from_row = move[1] - '1';
	int to_col = move[3] - 'a';
	int to_row = move[4] - '1';

	move_piece ((from_row << 4) | (from_col & 0xf), ((to_row << 4) | (to_col & 0xf)));
}