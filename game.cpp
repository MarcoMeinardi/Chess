#include "./game.h"

Game::Game () {
	board[COORD (0, 0)] = new Piece	(COORD (0, 0), WHITE, ROOK);
	board[COORD (0, 1)] = new Piece	(COORD (0, 1), WHITE, KNIGHT);
	board[COORD (0, 2)] = new Piece	(COORD (0, 2), WHITE, BISHOP);
	board[COORD (0, 3)] = new Piece	(COORD (0, 3), WHITE, QUEEN);
	board[COORD (0, 4)] = new Piece	(COORD (0, 4), WHITE, KING);
	board[COORD (0, 5)] = new Piece	(COORD (0, 5), WHITE, BISHOP);
	board[COORD (0, 6)] = new Piece	(COORD (0, 6), WHITE, KNIGHT);
	board[COORD (0, 7)] = new Piece	(COORD (0, 7), WHITE, ROOK);

	for (int i = 0; i < 8; i++) {
		board[COORD (1, i)] = new Piece (COORD (1, i), WHITE, PAWN);
	}

	for (int i = 2; i < 6; i++) {
		for (int j = 0; j < 8; j++) {
			board[COORD (i, j)] = nullptr;
		}
	}

	for (int i = 0; i < 8; i++) {
		board[COORD (6, i)] = new Piece (COORD (6, i), BLACK, PAWN);
	}
    
	board[COORD (7, 0)] = new Piece (COORD (7, 0), BLACK, ROOK);
	board[COORD (7, 1)] = new Piece (COORD (7, 1), BLACK, KNIGHT);
	board[COORD (7, 2)] = new Piece (COORD (7, 2), BLACK, BISHOP);
	board[COORD (7, 3)] = new Piece (COORD (7, 3), BLACK, QUEEN);
	board[COORD (7, 4)] = new Piece (COORD (7, 4), BLACK, KING);
	board[COORD (7, 5)] = new Piece (COORD (7, 5), BLACK, BISHOP);
	board[COORD (7, 6)] = new Piece (COORD (7, 6), BLACK, KNIGHT);
	board[COORD (7, 7)] = new Piece (COORD (7, 7), BLACK, ROOK);

	turn = WHITE;
	last_moved = -1;

	remaining_pieces[WHITE] = 16;
	remaining_pieces[BLACK] = 16;
	moves_without_take_or_pawn_move = 0;

	is_checkmate = false;
	is_draw = false;

	kings[WHITE] = board [COORD (0, 4)];
	kings[BLACK] = board [COORD (7, 4)];
}
Game::~Game () {
	for (int i = 0; i < 64; i++) {
		if (board[i]) {
			free (board[i]);
		}
	}
}

void Game::move_piece (int from, int to) {
	moves_without_take_or_pawn_move++;
	turn = !turn;

	if (board[from]->get_type () == KING) {
		// short-castle
		if (to == from + 2 * RIGHT) {
			board[from]->move (to);
			board[to] = board[from];
			board[from] = nullptr;
			board[from + 3 * RIGHT]->move (from + 1 * RIGHT);
			board[from + 1 * RIGHT] = board[from + 3];
			board[from + 3 * RIGHT] = nullptr;
			return;
		} else 
		// long castle
		if (to == from + 2 * LEFT) {
			board[from]->move (to);
			board[to] = board[from];
			board[from] = nullptr;
			board[from + 4 * LEFT]->move (from + 1 * LEFT);
			board[from + 1 * LEFT] = board[from + 4 * LEFT];
			board[from + 4 * LEFT] = nullptr;
			return;
		}
	}

	// en passant
	if (board[from]->get_type () == PAWN && X (from) != X (to) && !board[CUT_PROMOTION (to)]) {
		free (board[(from & 0b111000) | X (to)]);	// COORD (Y(from), X (to))
		board[(from & 0b111000) | X (to)] = nullptr;
		remaining_pieces[turn]--;
	} else 
	// promotion
	if (GET_PROMOTION (to)) {
		board[from]->promote (GET_PROMOTION (to));
	}

	to = CUT_PROMOTION (to);
	last_moved = to;

	if (board[from]->get_type () == PAWN) {
		moves_without_take_or_pawn_move = 0;
	}

	if (board[to]) {
		free (board[to]);
		moves_without_take_or_pawn_move = 0;
		remaining_pieces[turn]--;
		check_draw ();
	}
	board[from]->move (to);
	board[to] = board[from];
	board[from] = nullptr;

	if (moves_without_take_or_pawn_move == 100) {
		is_draw = true;
	}
}
int Game::load_moves (int* moves) {
	int* original_moves = moves;
	int n_moves;

	for (int i = 0; i < 64; i++) {	// y = i >> 3, x = i & 0x7
		if (board[i] && board[i]->get_owner () == turn) {
			switch (board[i]->get_type ()) {
			case BISHOP:
				get_bishop_moves	(i, moves);
				break;
			case KING:
				get_king_moves		(i, moves);
				break;
			case KNIGHT:
				get_knight_moves	(i, moves);
				break;
			case PAWN:
				get_pawn_moves		(i, moves);
				break;
			case QUEEN:
				get_queen_moves		(i, moves);
				break;
			case ROOK:
				get_rook_moves		(i, moves);
				break;
			}
		}
	}

	n_moves = moves - original_moves;
	if (n_moves == 0) {
		if (is_check (kings[turn]->get_pos ())) {
			is_checkmate = true;
		} else {
			is_draw = true;
		}
	}
	return n_moves;
}
void Game::add_move (int from, int to, int*& moves, Piece* eaten = nullptr) {
	// simulate move
	if (eaten) {
		board[eaten->get_pos ()] = nullptr;
	}
	// promote
	if (GET_PROMOTION (to)) {
		board[from]->promote (GET_PROMOTION (to));
	}
	board[from]->set_pos (CUT_PROMOTION (to));
	board[CUT_PROMOTION (to)] = board[from];
	board[from] = nullptr;

	if (!is_check (kings[turn]->get_pos ())) {
		*(moves++) = COMPRESS_MOVE (from, to);
	}

	// undo move
	board[CUT_PROMOTION (to)]->set_pos (from);
	board[from] = board[CUT_PROMOTION (to)];
	board[CUT_PROMOTION (to)] = nullptr;
	if (eaten) {
		board[eaten->get_pos ()] = eaten;
	}
	// undo promote
	if (GET_PROMOTION (to)) {
		board[from]->undo_promote ();
	}
}
bool Game::is_check (int pos) {
	return checked_diagonal (pos) || checked_horizontal (pos) || checked_by_knight (pos);
}
void Game::check_draw () {
	if (remaining_pieces[WHITE] > 1 && remaining_pieces[BLACK] > 1) {
		return;
	}
	if (remaining_pieces[WHITE] == 1 && remaining_pieces[BLACK] == 1) {
		is_draw = true;
		return;
	}

	Piece* P;
	if (remaining_pieces[WHITE] == 2) {
		for (int i = 0; i < 64; i++) {
			P = board[i];
			if (P && P->get_owner () == WHITE) {
				if (P->get_type () & (BISHOP | KNIGHT)) {
					is_draw = true;
					return;
				}
			}
		}
	} else {
		for (int i = 0; i < 64; i++) {
			P = board[i];
			if (P && P->get_owner () == BLACK) {
				if (P->get_type () & (BISHOP | KNIGHT)) {
					is_draw = true;
					return;
				}
			}
		}
	}
}

void Game::get_bishop_moves (int pos, int*& moves) {
	int p;
	Piece* P;

	// rigth-up
	p = pos;
	while (Y (p) != 0b111 && X (p) != 0b111) {
		p += RIGHT_UP;
		P = board[p];
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, moves, P);
			}
			break;
		}
		add_move (pos, p, moves);
	}

	// right-down
	p = pos;
	while (Y (p) != 0b000 && X (p) != 0b111) {
		p += RIGHT_DOWN;
		P = board [p];
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, moves, P);
			}
			break;
		}
		add_move (pos, p, moves);
	}

	// left-down
	p = pos;
	while (Y (p) != 0b000 && X (p) != 0b000) {
		p += LEFT_DOWN;
		P = board [p];
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, moves, P);
			}
			break;
		}
		add_move (pos, p, moves);
	}

	// left-up
	p = pos;
	while (Y (p) != 0b111 && X (p) != 0b000) {
		p += LEFT_UP;
		P = board[p];
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, moves, P);
			}
			break;
		}
		add_move (pos, p, moves);
	}
}
void Game::get_king_moves (int pos, int*& moves) {	int p;
	Piece* P;

	// up
	if (Y (pos) != 0b111) {
		p = pos + UP;
		P = board[p];
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// right-up
	if (Y (pos) != 0b111 && X (pos) != 0b111) {
		p = pos + RIGHT_UP; 
		P = board[p];
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// right
	if (X (pos) != 0b111) {
		p = pos + RIGHT;
		P = board[p];
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// right-down
	if (Y (pos) != 0b000 && X (pos) != 0b111) {
		p = pos + RIGHT_DOWN;
		P = board[p];
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// down
	if (Y (pos) != 0b000) {
		p = pos + DOWN;
		P = board[p];
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// left-down
	if (Y (pos) != 0b000 && X (pos) != 0b000) {
		p = pos + LEFT_DOWN;
		P = board[p];
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// left
	if (X (pos) != 0b000) {
		p = pos + LEFT;
		P = board[p];
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// left-up
	if (Y (pos) != 0b111 && X (pos) != 0b000) {
		p = pos + LEFT_UP;
		P = board[p];
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// short-castle
	if (board[pos]->is_first_move () && !board[pos + 1 * RIGHT] && !board[pos + 2 * RIGHT]) {
		P = board[pos + 3 * RIGHT];
		if (P && P->get_type () == ROOK && P->is_first_move()) {
			if (!is_check (pos) && !is_check (pos + 1 * RIGHT)) {
				add_move (pos, pos + 2 * RIGHT, moves);
			}
		}
	}
	// long-castle
	if (board[pos]->is_first_move () && !board[pos + 1 * LEFT] && !board[pos + 2 * LEFT] && !board[pos + 3 * LEFT]) {
		P = board[pos + 4 * LEFT];
		if (P && P->get_type () == ROOK && P->is_first_move()) {
			if (!is_check (pos) && !is_check (pos + 1 * LEFT)) {
				add_move (pos, pos + 2 * LEFT, moves);
			}
		}
	}
}
void Game::get_knight_moves (int pos, int*& moves) {
	int p;
	Piece* P;

	// up-right
	if (Y (pos) < 0b110 && X (pos) != 0b111) {
		p = pos + UP * 2 + RIGHT;
		P = board[p];
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// right-up
	if (Y (pos) != 0b111 && X (pos) < 0b110) {
		p = pos + RIGHT * 2 + UP;
		P = board[p];
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// right-down
	if (Y (pos) != 0b000 && X (pos) < 0b110) {
		p = pos + RIGHT * 2 + DOWN;
		P = board[p];
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// down-right
	if (Y (pos) > 0b001 && X (pos) != 0b111) {
		p = pos + DOWN * 2 + RIGHT;
		P = board[p];
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// down-left
	if (Y (pos) > 0b001 && X (pos) != 0b000) {
		p = pos + DOWN * 2 + LEFT;
		P = board[p];
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// left-down
	if (Y (pos) != 0b000 && X (pos) > 0b001) {
		p = pos + LEFT * 2 + DOWN;
		P = board[p];
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// left-up
	if (Y (pos) != 0b111 && X (pos) > 0b001) {
		p = pos + LEFT * 2 + UP;
		P = board[p];
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// up-left
	if (Y (pos) < 0b110 && X (pos) != 0b000) {
		p = pos + UP * 2 + LEFT;
		P = board[p];
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}
}
void Game::get_pawn_moves (int pos, int*& moves) {
	int p;
	Piece* P;

	// don't need to check up, because of promotion
	if (turn == WHITE) {
		// move
		p = pos + UP;
		if (!board[p]) {
			if (Y (p) == 0b111) {
				add_move (pos, p | PROMOTE (QUEEN), moves);
				add_move (pos, p | PROMOTE (KNIGHT), moves);
				add_move (pos, p | PROMOTE (ROOK), moves);
				add_move (pos, p | PROMOTE (BISHOP), moves);
			} else {
				add_move (pos, p, moves);
				if (Y (pos) == 0b001) {
					p += UP;
					if (!board[p]) {
						add_move (pos, p, moves);
					}
				}
			}
		}

		// take right
		if (X (pos) != 0b111) {	
			p = pos + RIGHT_UP;
			P = board[p];
			if (P && P->get_owner () == BLACK) {
				if (Y (p) == 0b111) {
					add_move (pos, p | PROMOTE (QUEEN), moves, P);
					add_move (pos, p | PROMOTE (KNIGHT), moves, P);
					add_move (pos, p | PROMOTE (ROOK), moves, P);
					add_move (pos, p | PROMOTE (BISHOP), moves, P);
				} else {
					add_move (pos, p, moves, P);
				}
			}
		}
		// take left
		if (X (pos) != 0b000) {
			p = pos + LEFT_UP;
			P = board[p];
			if (P && P->get_owner () == BLACK) {
				if (Y (p) == 0b111) {
					add_move (pos, p | PROMOTE (QUEEN), moves, P);
					add_move (pos, p | PROMOTE (KNIGHT), moves, P);
					add_move (pos, p | PROMOTE (ROOK), moves, P);
					add_move (pos, p | PROMOTE (BISHOP), moves, P);
				} else {
					add_move (pos, p, moves, P);
				}
			}
		}

		// en passant
		if (X (pos) != 0b111 && last_moved == pos + RIGHT) {
			P = board[last_moved];
			if (P->get_owner () == BLACK && P->can_be_en_passant ()) {
				add_move (pos, pos + RIGHT_UP, moves, P);
			}
		} else if (X (pos) != 0b000 && last_moved == pos + LEFT) {
			P = board[last_moved];
			if (P->get_owner () == BLACK && P->can_be_en_passant ()) {
				add_move (pos, pos + LEFT_UP, moves, P);
			}
		}
	} else {
		// move
		p = pos + DOWN;
		if (!board[p]) {
			if (Y (p) == 0b000) {
				add_move (pos, p | PROMOTE (QUEEN), moves);
				add_move (pos, p | PROMOTE (KNIGHT), moves);
				add_move (pos, p | PROMOTE (ROOK), moves);
				add_move (pos, p | PROMOTE (BISHOP), moves);
			} else {
				add_move (pos, p, moves);
				if (Y (pos) == 0b110) {
					p += DOWN;
					if (!board[p]) {
						add_move (pos, p, moves);
					}
				}
			}
		}

		// take right
		if (X (pos) != 0b111) {	
			p = pos + RIGHT_DOWN;
			P = board[p];
			if (P && P->get_owner () == WHITE) {
				if (Y (p) == 0b000) {
					add_move (pos, p | PROMOTE (QUEEN), moves, P);
					add_move (pos, p | PROMOTE (KNIGHT), moves, P);
					add_move (pos, p | PROMOTE (ROOK), moves, P);
					add_move (pos, p | PROMOTE (BISHOP), moves, P);
				} else {
					add_move (pos, p, moves, P);
				}
			}
		}
		// take left
		if (X (pos) != 0b000) {
			p = pos + LEFT_DOWN;
			P = board[p];
			if (P && P->get_owner () == WHITE) {
				if (Y (p) == 0b000) {
					add_move (pos, p | PROMOTE (QUEEN), moves, P);
					add_move (pos, p | PROMOTE (KNIGHT), moves, P);
					add_move (pos, p | PROMOTE (ROOK), moves, P);
					add_move (pos, p | PROMOTE (BISHOP), moves, P);
				} else {
					add_move (pos, p, moves, P);
				}
			}
		}

		// en passant
		if (X (pos) != 0b111 && last_moved == pos + RIGHT) {
			P = board[last_moved];
			if (P->get_owner () == WHITE && P->can_be_en_passant ()) {
				add_move (pos, pos + RIGHT_DOWN, moves, P);
			}
		} else if (X (pos) != 0b000 && last_moved == pos + LEFT) {
			P = board[last_moved];
			if (P->get_owner () == WHITE && P->can_be_en_passant ()) {
				add_move (pos, pos + LEFT_DOWN, moves, P);
			}
		}
	}
}
void Game::get_queen_moves (int pos, int*& moves) {
	get_bishop_moves (pos, moves);
	get_rook_moves (pos, moves);
}
void Game::get_rook_moves (int pos, int*& moves) {
	int p;
	Piece* P;

	// up
	p = pos;
	while (Y (p) != 0b111) {
		p += UP;
		P = board[p];
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, moves, P);
			}
			break;
		}
		add_move (pos, p, moves);
	}

	// right
	p = pos;
	while (X (p) != 0b111) {
		p += RIGHT;
		P = board[p];
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, moves, P);
			}
			break;
		}
		add_move (pos, p, moves);
	}

	// down
	p = pos;
	while (Y (p) != 0b000) {
		p += DOWN;
		P = board[p];
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, moves, P);
			}
			break;
		}
		add_move (pos, p, moves);
	}

	// left
	p = pos;
	while (X (p) != 0b000) {
		p += LEFT;
		P = board[p];
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, moves, P);
			}
			break;
		}
		add_move (pos, p, moves);
	}
}

bool Game::checked_diagonal (int pos) {
	int p;
	Piece* P;

	// rigth-up
	if (Y (pos) != 0b111 && X (pos) != 0b111) {
		p = pos + RIGHT_UP;
		P = board[p];
		if (P) {
			if (P->get_owner () != turn) {
				if (P->get_type () & (BISHOP | QUEEN | KING)) {
					return true;
				} else if (turn == WHITE && P->get_type () == PAWN) {
					return true;
				}
			}
		} else {
			while (Y (p) != 0b111 && X (p) != 0b111) {
				p += RIGHT_UP;
				P = board[p];
				if (P) {
					if (P->get_owner () != turn && (P->get_type () & (BISHOP | QUEEN))) {
						return true;
					}
					break;
				}
			}
		}
	}

	// right-down
	if (Y (pos) != 0b000 && X (pos) != 0b111) {
		p = pos + RIGHT_DOWN;
		P = board[p];
		if (P) {
			if (P->get_owner () != turn) {
				if (P->get_type () & (BISHOP | QUEEN | KING)) {
					return true;
				} else if (turn == BLACK && P->get_type () == PAWN) {
					return true;
				}
			}
		} else {
			while (Y (p) != 0b000 && X (p) != 0b111) {
				p += RIGHT_DOWN;
				P = board[p];
				if (P) {
					if (P->get_owner () != turn && (P->get_type () & (BISHOP | QUEEN))) {
						return true;
					}
					break;
				}
			}
		}
	}

	// left-down
	if (Y (pos) != 0b000 && X (pos) != 0b000) {
		p = pos + LEFT_DOWN;
		P = board[p];
		if (P) {
			if (P->get_owner () != turn) {
				if (P->get_type () & (BISHOP | QUEEN | KING)) {
					return true;
				} else if (turn == BLACK && P->get_type () == PAWN) {
					return true;
				}
			}
		} else {
			while (Y (p) != 0b000 && X (p) != 0b000) {
				p += LEFT_DOWN;
				P = board[p];
				if (P) {
					if (P->get_owner () != turn && (P->get_type () & (BISHOP | QUEEN))) {
						return true;
					}
					break;
				}
			}
		}
	}

	// left-up
	if (Y (pos) != 0b111 && X (pos) != 0b000) {
		p = pos + LEFT_UP;
		P = board[p];
		if (P) {
			if (P->get_owner () != turn) {
				if (P->get_type () & (BISHOP | QUEEN | KING)) {
					return true;
				} else if (turn == WHITE && P->get_type () == PAWN) {
					return true;
				}
			}
		} else {
			while (Y (p) != 0b111 && X (p) != 0b000) {
				p += LEFT_UP;
				P = board[p];
				if (P) {
					if (P->get_owner () != turn && (P->get_type () & (BISHOP | QUEEN))) {
						return true;
					}
					break;
				}
			}
		}
	}

	return false;
}
bool Game::checked_horizontal (int pos) {
	int p;
	Piece* P;

	// up
	if (Y (pos) != 0b111) {
		p = pos + UP;
		P = board[p];
		if (P) {
			if (P->get_owner () != turn && P->get_type () & (ROOK | QUEEN | KING)) {
				return true;
			}
		} else {
			while (Y (p) != 0b111) {
				p += UP;
				P = board[p];
				if (P) {
					if (P->get_owner () != turn && (P->get_type () & (ROOK | QUEEN))) {
						return true;
					}
					break;
				}
			}
		}
	}

	// right
	if (X (pos) != 0b111) {
		p = pos + RIGHT;
		P = board[p];
		if (P) {
			if (P->get_owner () != turn && P->get_type () & (ROOK | QUEEN | KING)) {
				return true;
			}
		} else {
			while (X (p) != 0b111) {
				p += RIGHT;
				P = board[p];
				if (P) {
					if (P->get_owner () != turn && (P->get_type () & (ROOK | QUEEN))) {
						return true;
					}
					break;
				}
			}
		}
	}

	// down
	if (Y (pos) != 0b000) {
		p = pos + DOWN;
		P = board[p];
		if (P) {
			if (P->get_owner () != turn && P->get_type () & (ROOK | QUEEN | KING)) {
				return true;
			}
		} else {
			while (Y (p) != 0b000) {
				p += DOWN;
				P = board[p];
				if (P) {
					if (P->get_owner () != turn && (P->get_type () & (ROOK | QUEEN))) {
						return true;
					}
					break;
				}
			}
		}
	}

	// left
	if (X (pos) != 0b000) {
		p = pos + LEFT;
		P = board[p];
		if (P) {
			if (P->get_owner () != turn && P->get_type () & (ROOK | QUEEN | KING)) {
				return true;
			}
		} else {
			while (X (p) != 0b000) {
				p += LEFT;
				P = board[p];
				if (P) {
					if (P->get_owner() != turn && (P->get_type() & (ROOK | QUEEN))) {
						return true;
					}
					break;
				}
			}
		}
	}

	return false;
}
bool Game::checked_by_knight (int pos) {
	Piece* P;

	// up-right
	if (Y (pos) < 0b110 && X (pos) != 0b111) {
		P = board[pos + UP * 2 + RIGHT];
		if (P && P->get_owner() != turn && P->get_type() == KNIGHT) {
			return true;
		}
	}

	// right-up
	if (Y (pos) != 0b111 && X (pos) < 0b110) {
		P = board[pos + RIGHT * 2 + UP];
		if (P && P->get_owner() != turn && P->get_type() == KNIGHT) {
			return true;
		}
	}

	// right-down
	if (Y (pos) != 0b000 && X (pos) < 0b110) {
		P = board[pos + RIGHT * 2 + DOWN];
		if (P && P->get_owner() != turn && P->get_type() == KNIGHT) {
			return true;
		}
	}

	// down-right
	if (Y (pos) > 0b001 && X (pos) != 0b111) {
		P = board[pos + DOWN * 2 + RIGHT];
		if (P && P->get_owner() != turn && P->get_type() == KNIGHT) {
			return true;
		}
	}

	// down-left
	if (Y (pos) > 0b001 && X (pos) != 0b000) {
		P = board[pos + DOWN * 2 + LEFT];
		if (P && P->get_owner() != turn && P->get_type() == KNIGHT) {
			return true;
		}
	}

	// left-down
	if (Y (pos) != 0b000 && X (pos) > 0b001) {
		P = board[pos + LEFT * 2 + DOWN];
		if (P && P->get_owner() != turn && P->get_type() == KNIGHT) {
			return true;
		}
	}

	// left-up
	if (Y (pos) != 0b111 && X (pos) > 0b001) {
		P = board[pos + LEFT * 2 + UP];
		if (P && P->get_owner() != turn && P->get_type() == KNIGHT) {
			return true;
		}
	}

	// up-left
	if (Y (pos) < 0b110 && X (pos) != 0b000) {
		P = board[pos + UP * 2 + LEFT];
		if (P && P->get_owner() != turn && P->get_type() == KNIGHT) {
			return true;
		}
	}

	return false;
}

int Game::get_piece_moves_GUI (int pos, int* moves) {
	int* original_moves = moves;
	if (!board[pos] || board[pos]->get_owner () != turn || is_checkmate || is_draw) {
		return 0;
	}

	switch (board[pos]->get_type ()) {
	case BISHOP:
		get_bishop_moves	(pos, moves);
		break;
	case KING:
		get_king_moves		(pos, moves);
		break;
	case KNIGHT:
		get_knight_moves	(pos, moves);
		break;
	case PAWN:
		get_pawn_moves		(pos, moves);
		break;
	case QUEEN:
		get_queen_moves		(pos, moves);
		break;
	case ROOK:
		get_rook_moves		(pos, moves);
		break;
	}

	return moves - original_moves;
}
string Game::move_piece_GUI (int from, int to) {
	// castle
	if (board[from]->get_type () == KING && (X (to) == X (from) + 2 || X (to) == X (from) - 2)) {
		move_piece (from, to);
		return "O";
	}
	// en passant
	int eaten = -1;
	if (board[from]->get_type () == PAWN && X (from) != X (to) && !board[CUT_PROMOTION (to)]) {
		eaten = COORD (Y (from), X (to));
	}
	if (board[CUT_PROMOTION (to)]) {
		eaten = CUT_PROMOTION (to);
	}
	move_piece (from, to);
	return to_string (eaten);
}

void Game::print_board () {
	for (int i = 7; i >= 0; i--) {
		cout << "\t" << (i + 1) << "  |";
		for (int j = 0; j < 8; j++) {
			cout << "\t";
			if (board[COORD (i, j)]) {
				cout << pieces_repr[board[COORD (i, j)]->get_type ()];
				cout << (board[COORD (i, j)]->get_owner () == WHITE ? 'W' : 'B');
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
void Game::print_possible_moves (int* moves, int n_moves) {
	cout << n_moves << endl;
	if (is_checkmate) {
		cout << "Checkmate" << endl;
	} else if (is_draw) {
		cout << "Draw" << endl;
	} else {
		for (int i = 0; i < n_moves; i++) {
			int move = moves[i];
			int DECOMPRESS_MOVE (move, from, to);
			cout << pieces_repr[board[from]->get_type ()] << "\t";
			cout << (char)(X (from)	+ 'a') << (Y (from)					+ 1) << "\t->\t";
			cout << (char)(X (to)	+ 'a') << (Y (CUT_PROMOTION (to))	+ 1);
			if (GET_PROMOTION (to)) {
				cout << pieces_repr[GET_PROMOTION (to)];
			}
			cout << endl;
		}
	}
}
void Game::human_move (string move) {	// "a1 h8x" or "A"
	if (move[0] == 'A') {
		int best_move;
		int score = get_best_move (&best_move);
		int DECOMPRESS_MOVE (best_move, from, to);

		cout << "Predicted score: " << score << endl;
		cout << pieces_repr[board[from]->get_type ()] << "\t";
		cout << (char)(X (from)	+ 'a') << (Y (from)					+ 1) << "\t->\t";
		cout << (char)(X (to)	+ 'a') << (Y (CUT_PROMOTION (to))	+ 1);
		if (GET_PROMOTION (to)) {
			cout << pieces_repr[GET_PROMOTION (to)];
		}
		cout << endl << endl;

		return;
	}
	int from_col =	move[0] - 'a';
	int from_row =	move[1] - '1';
	int to_col =	move[3] - 'a';
	int to_row =	move[4] - '1';
	int new_type = 0;
	if (move[5]) {
		for (new_type = 1; new_type < 34; new_type++) {
			if (move[5] == pieces_repr[new_type]) {
				break;
			}
		}
	}

	move_piece (COORD (from_row, from_col), COORD (to_row, to_col) | PROMOTE (new_type));
}

int Game::test () {
	int cnt = 0;
	int move;
	int moves[128];
	int n_moves;

	while (cnt < 10'000) {
		n_moves = load_moves (moves);
		if (is_checkmate || is_draw) {
			break;
		}
		move = moves[rand () % n_moves];
		// print_board ();
		// int DECOMPRESS_MOVE (move, from, to);
		// cout << pieces_repr[board[from]->get_type ()] << "\t";
		// cout << (char)(X (from)	+ 'a') << (Y (from)					+ 1) << "\t->\t";
		// cout << (char)(X (to)	+ 'a') << (Y (CUT_PROMOTION (to))	+ 1);
		// if (GET_PROMOTION (to)) {
		// 	cout << pieces_repr[GET_PROMOTION (to)];
		// }
		// cout << endl;

		move_piece (move & 0xff, move >> 8);
		cnt++;
		if (is_draw) {
			break;
		}
	}

	return cnt | (turn << 29) | (is_checkmate << 30) | (is_draw << 31);
}

int Game::get_best_move (int* best_move) {
	if (turn == WHITE) {
		return maxi (MAX_DEPTH, -INF, INF, best_move);
	} else {
		return mini (MAX_DEPTH, -INF, INF, best_move);
	}
}
int Game::mini (int depth, int alpha, int beta, int* best_move) {
	if (!depth) {
		// return evaluate score
		return 0;
	}

	int moves[128];
	int n_moves = load_moves (moves);

	if (is_checkmate) {
		return INF;
	} else if (is_draw) {
		return 0;
	}
	random_shuffle (moves, moves + n_moves);	// in the beginning the predicted score is zero so he just make the first available move

	int best_score = INF;
	int actual_score;
	int my_best_move;
	Piece* eaten;
	Piece* moved;
	int prev_moves_without_take_or_pawn_move = moves_without_take_or_pawn_move;
	int prev_last_moved = last_moved;
	bool was_first_double_move;
	bool had_moved;
	int previous_type;	// promotion score

	for (int i = 0; i < n_moves; i++) {
		// simulate move
		moved = board[CUT_PROMOTION (moves[i])];
		was_first_double_move = moved->can_be_en_passant ();
		had_moved = moved->is_first_move ();
		previous_type = moved->get_type ();
		eaten = simulate_move (moves[i]);
		if (eaten) {
			actual_score = values[eaten->get_type ()];
		} else {
			actual_score = 0;
		}
		if (previous_type == PAWN && moved->get_type () != PAWN) {
			actual_score -= values[moved->get_type ()] - 1;
		}

		if (is_draw) {
			actual_score = 0;
		} else {
			actual_score += maxi (depth - 1, alpha, beta, nullptr);
		}
		if (actual_score < best_score) {
			best_score = actual_score;
			my_best_move = moves[i];
			if (best_score <= alpha) {
				// undo simulated move
				undo_simulated_move (moves[i], eaten);
				moves_without_take_or_pawn_move = prev_moves_without_take_or_pawn_move;
				last_moved = prev_last_moved;
				moved->set_prev_first_double_move (was_first_double_move);
				if (!had_moved) {
					moved->undo_first_move ();
				}
				break;
			}
			if (best_score > beta) {
				beta = best_score;
			}
		}

		// undo simulated move
		undo_simulated_move (moves[i], eaten);
		moves_without_take_or_pawn_move = prev_moves_without_take_or_pawn_move;
		last_moved = prev_last_moved;
		moved->set_prev_first_double_move (was_first_double_move);
		if (!had_moved) {
			moved->undo_first_move ();
		}
	}

	if (best_move) {
		*best_move = my_best_move;
	}
	return best_score;
}
int Game::maxi (int depth, int alpha, int beta, int* best_move) {
	if (!depth) {
		// return evaluate score
		return 0;
	}

	int moves[128];
	int n_moves = load_moves (moves);

	if (is_checkmate) {
		return -INF;
	} else if (is_draw) {
		return 0;
	}
	random_shuffle (moves, moves + n_moves);	// in the beginning the predicted score is zero so he just make the first available move

	int best_score = -INF;
	int actual_score;
	int my_best_move;
	Piece* eaten;
	Piece* moved;
	int prev_moves_without_take_or_pawn_move = moves_without_take_or_pawn_move;
	int prev_last_moved = last_moved;
	bool was_first_double_move;
	bool had_moved;
	int previous_type;	// promotion score

	for (int i = 0; i < n_moves; i++) {
		// simulate move
		moved = board[CUT_PROMOTION (moves[i])];
		was_first_double_move = moved->can_be_en_passant ();
		had_moved = moved->is_first_move ();
		previous_type = moved->get_type ();
		eaten = simulate_move (moves[i]);
		if (eaten) {
			actual_score = values[eaten->get_type ()];
		} else {
			actual_score = 0;
		}
		if (previous_type == PAWN && moved->get_type () != PAWN) {
			actual_score += values[moved->get_type ()] - 1;
		}

		if (is_draw) {
			actual_score = 0;
		} else {
			actual_score += mini (depth - 1, alpha, beta, nullptr);
		}
		if (actual_score > best_score) {
			best_score = actual_score;
			my_best_move = moves[i];
			if (best_score >= beta) {
				// undo simulated move
				undo_simulated_move (moves[i], eaten);
				moves_without_take_or_pawn_move = prev_moves_without_take_or_pawn_move;
				last_moved = prev_last_moved;
				moved->set_prev_first_double_move (was_first_double_move);
				if (!had_moved) {
					moved->undo_first_move ();
				}
				break;
			}
			if (best_score < alpha) {
				alpha = best_score;
			}
		}

		// undo simulated move
		undo_simulated_move (moves[i], eaten);
		moves_without_take_or_pawn_move = prev_moves_without_take_or_pawn_move;
		last_moved = prev_last_moved;
		moved->set_prev_first_double_move (was_first_double_move);
		if (!had_moved) {
			moved->undo_first_move ();
		}
	}

	if (best_move) {
		*best_move = my_best_move;	// my_best_move cannot be uninitialized because it would be checkmate or draw
	}
	return best_score;
}
Piece* Game::simulate_move (int move) {
	int DECOMPRESS_MOVE (move, from, to);
	moves_without_take_or_pawn_move++;
	turn = !turn;
	Piece* eaten = nullptr;

	if (board[from]->get_type () == KING) {
		// short-castle
		if (to == from + 2 * RIGHT) {
			board[from]->move (to);
			board[to] = board[from];
			board[from] = nullptr;
			board[from + 3 * RIGHT]->move (from + 1 * RIGHT);
			board[from + 1 * RIGHT] = board[from + 3 * RIGHT];
			board[from + 3 * RIGHT] = nullptr;
			return nullptr;
		} else 
		// long castle
		if (to == from + 2 * LEFT) {
			board[from]->move (to);
			board[to] = board[from];
			board[from] = nullptr;
			board[from + 4 * LEFT]->move (from + 1 * LEFT);
			board[from + 1 * LEFT] = board[from + 4 * LEFT];
			board[from + 4 * LEFT] = nullptr;
			return nullptr;
		}
	}

	// en passant
	if (board[from]->get_type () == PAWN && X (from) != X (to) && !board[CUT_PROMOTION (to)]) {
		eaten = board[(from & 0b111000) | X (to)];
		board[(from & 0b111000) | X (to)] = nullptr;
		remaining_pieces[turn]--;
	} else 
	// promotion
	if (GET_PROMOTION (to)) {
		board[from]->promote (GET_PROMOTION (to));
	}

	to = CUT_PROMOTION (to);
	last_moved = to;

	if (board[from]->get_type () == PAWN) {
		moves_without_take_or_pawn_move = 0;
	}

	if (board[to]) {
		eaten = board[to];
		moves_without_take_or_pawn_move = 0;
		remaining_pieces[turn]--;
		check_draw ();
	}
	board[from]->move (to);
	board[to] = board[from];
	board[from] = nullptr;

	if (moves_without_take_or_pawn_move == 100) {
		is_draw = true;
	}

	return eaten;
}
void Game::undo_simulated_move (int move, Piece* eaten) {
	int DECOMPRESS_MOVE (move, from, to);

	if (board[CUT_PROMOTION (to)]->get_type () == KING) {
		// short-castle
		if (to == from + 2 * RIGHT) {
			board[to]->move (from);
			board[from] = board[to];
			board[to] = nullptr;
			board[from + 1 * RIGHT]->move (from + 3 * RIGHT);
			board[from + 3 * RIGHT] = board[from + 1 * RIGHT];
			board[from + 1 * RIGHT] = nullptr;
			board[from + 3 * RIGHT]->undo_first_move ();
			is_checkmate = false;
			is_draw = false;
			turn = !turn;
			return;
		} else 
		// long castle
		if (to == from + 2 * LEFT) {
			board[to]->move (from);
			board[from] = board[to];
			board[to] = nullptr;
			board[from + 1 * LEFT]->move (from + 4 * LEFT);
			board[from + 4 * LEFT] = board[from + 1 * LEFT];
			board[from + 1 * LEFT] = nullptr;
			board[from + 4 * LEFT]->undo_first_move ();
			is_checkmate = false;
			is_draw = false;
			turn = !turn;
			return;
		}
	}

	// promotion
	if (GET_PROMOTION (to)) {
		board[CUT_PROMOTION (to)]->undo_promote ();
	}

	to = CUT_PROMOTION (to);

	board[to]->move (from);
	board[from] = board[to];
	board[to] = nullptr;

	if (eaten) {
		remaining_pieces[turn]++;
		board[eaten->get_pos ()] = eaten;
	}

	turn = !turn;
	is_checkmate = false;
	is_draw = false;
}
