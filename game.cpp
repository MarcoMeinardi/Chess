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
		board[(6 << 3) | (i & 0x7)] = new Piece ((6 << 4) | (i & 0xf), BLACK, PAWN);
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

	remaining_pieces[WHITE] = 16;
	remaining_pieces[BLACK] = 16;
	moves_without_take_or_pawn_move = 0;

	is_checkmate = false;
	is_draw = false;

	kings[WHITE] = board [(0 << 3) | (4 & 0x7)];
	kings[BLACK] = board [(7 << 3) | (4 & 0x7)];
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
		} else 
		// long castle
		if (to == from - 2) {
			board(from)->move (to);
			board(to) = board(from);
			board(from) = nullptr;
			board(from - 4)->move (from - 1);
			board(from - 1) = board(from - 4);
			board(from - 4) = nullptr;
			return;
		}
	}

	// en passant
	if (board(from)->get_type () == PAWN && (from & 0x7) != (to & 0x7) && !board(to & 0xff)) {
		free (board[((from >> 1) & 0b111000) | (to & 0b000111)]);
		board[((from >> 1) & 0b111000) | (to & 0b000111)] = nullptr;
		remaining_pieces[turn]--;
	} else 
	// promotion
	if (to >> 8) {
		board(from)->promote (to >> 8);
	}

	to &= 0xff;
	last_moved = to;

	if (board(from)->get_type () == PAWN) {
		moves_without_take_or_pawn_move = 0;
	}

	if (board(to)) {
		free (board(to));
		moves_without_take_or_pawn_move = 0;
		remaining_pieces[turn]--;
		check_draw ();
	}
	board(from)->move (to);
	board(to) = board(from);
	board(from) = nullptr;

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
				get_bishop_moves (((i << 1) & 0b1110000) | (i & 0b111), moves);
				break;
			case KING:
				get_king_moves (((i << 1) & 0b1110000) | (i & 0b111), moves);
				break;
			case KNIGHT:
				get_knight_moves (((i << 1) & 0b1110000) | (i & 0b111), moves);
				break;
			case PAWN:
				get_pawn_moves (((i << 1) & 0b1110000) | (i & 0b111), moves);
				break;
			case QUEEN:
				get_queen_moves (((i << 1) & 0b1110000) | (i & 0b111), moves);
				break;
			case ROOK:
				get_rook_moves (((i << 1) & 0b1110000) | (i & 0b111), moves);
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
		board(eaten->get_pos ()) = nullptr;
	}
	// promote
	if (to >> 8) {
		board(from)->promote (to >> 8);
	}
	board(from)->set_pos (to & 0xff);
	board(to & 0xff) = board(from);
	board(from) = nullptr;

	if (!is_check (kings[turn]->get_pos ())) {
		*(moves++) = from | (to << 8);
	}

	// undo move
	board(to & 0xff)->set_pos (from);
	board(from) = board(to & 0xff);
	board(to & 0xff) = nullptr;
	if (eaten) {
		board(eaten->get_pos ()) = eaten;
	}
	// undo promote
	if (to >> 8) {
		board(from)->undo_promote ();
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
	p = pos + ((1 << 4) | (1 & 0xf));
	while (!(p & 0b10001000)) {
		P = board(p);
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, moves, P);
			}
			break;
		}
		add_move (pos, p, moves);
		p += (1 << 4) | (1 & 0xf);
	}

	// right-down
	p = pos + (-(1 << 4) | (1 & 0xf));
	while (!(p & 0b10001000)) {
		P = board (p);
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, moves, P);
			}
			break;
		}
		add_move (pos, p, moves);
		p += -(1 << 4) | (1 & 0xf);
	}

	// left-down
	p = pos + (-(1 << 4) - (1 & 0xf));
	while (!(p & 0b10001000)) {
		P = board (p);
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, moves, P);
			}
			break;
		}
		add_move (pos, p, moves);
		p += -(1 << 4) - (1 & 0xf);
	}

	// left-up
	p = pos + ((1 << 4) - (1 & 0xf));
	while (!(p & 0b10001000)) {
		P = board(p);
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, moves, P);
			}
			break;
		}
		add_move (pos, p, moves);
		p += (1 << 4) - (1 & 0xf);
	}
}
void Game::get_king_moves (int pos, int*& moves) {
	int p;
	Piece* P;

	// up
	p = pos + (1 << 4);
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// up-right
	p = pos + ((1 << 4) | (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// right
	p = pos + 1;
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// down-right
	p = pos + (-(1 << 4) | (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// down
	p = pos - (1 << 4);
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// down-left
	p = pos + (-(1 << 4) - (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// left
	p = pos - 1;
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// left-up
	p = pos + ((1 << 4) - (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// short-castle
	if (board(pos)->is_first_move () && !board(pos + 1) && !board(pos + 2)) {
		P = board(pos + 3);
		if (P && P->get_type () == ROOK && P->is_first_move()) {
			if (!is_check (pos) && !is_check (pos + 1)) {
				add_move (pos, pos + 2, moves);
			}
		}
	}
	// long-castle
	if (board(pos)->is_first_move () && !board(pos - 1) && !board(pos - 2) && !board(pos - 3)) {
		P = board(pos - 4);
		if (P && P->get_type () == ROOK && P->is_first_move()) {
			if (!is_check (pos) && !is_check (pos - 1)) {
				add_move (pos, pos - 2, moves);
			}
		}
	}
}
void Game::get_knight_moves (int pos, int*& moves) {
	int p;
	Piece* P;

	// up-right
	p = pos + ((2 << 4) | (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// right-up
	p = pos + ((1 << 4) | (2 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// right-down
	p = pos + (-(1 << 4) | (2 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// down-right
	p = pos + (-(2 << 4) | (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// down-left
	p = pos + (-(2 << 4) - (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// left-down
	p = pos + (-(1 << 4) - (2 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// left-up
	p = pos + ((1 << 4) - (2 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}

	// up-left
	p = pos + ((2 << 4) - (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, moves, P);
		}
	}
}
void Game::get_pawn_moves (int pos, int*& moves) {
	int p;
	Piece* P;
	if (turn == WHITE) {
		// move
		p = pos + (1 << 4);
		if (!(p & 0b10001000) && !board(p)) {
			if ((p >> 4) == 7) {
				add_move (pos, p | (QUEEN << 8), moves);
				add_move (pos, p | (KNIGHT << 8), moves);
				add_move (pos, p | (ROOK << 8), moves);
				add_move (pos, p | (BISHOP << 8), moves);
			} else {
				add_move (pos, p, moves);
				p += (1 << 4);
				if ((pos >> 4) == 1  && !board(p)) {
					add_move (pos, p, moves);	
				}
			}
		}

		// take
		p = pos + ((1 << 4) | (1 & 0xf));
		if (!(p & 0b10001000)) {
			P = board(p);
			if (P && P->get_owner () == BLACK) {
				if ((p >> 4) == 7) {
					add_move (pos, p | (QUEEN << 8), moves, P);
					add_move (pos, p | (KNIGHT << 8), moves, P);
					add_move (pos, p | (ROOK << 8), moves, P);
					add_move (pos, p | (BISHOP << 8), moves, P);
				} else {
					add_move (pos, p, moves, P);
				}
			}
		}
		p = pos + ((1 << 4) - (1 & 0xf));
		if (!(p & 0b10001000)) {
			P = board(p);
			if (P && P->get_owner () == BLACK) {
				if ((p >> 4) == 7) {
					add_move (pos, p | (QUEEN << 8), moves, P);
					add_move (pos, p | (KNIGHT << 8), moves, P);
					add_move (pos, p | (ROOK << 8), moves, P);
					add_move (pos, p | (BISHOP << 8), moves, P);
				} else {
					add_move (pos, p, moves, P);
				}
			}
		}

		// en passant
		if (last_moved == pos + 1) {
			P = board(last_moved);
			if (P->get_owner () == BLACK && P->can_be_en_passant ()) {
				add_move (pos, pos + ((1 << 4) | 1), moves, P);
			}
		} else if (last_moved == pos - 1) {
			P = board(last_moved);
			if (P->get_owner () == BLACK && P->can_be_en_passant ()) {
				add_move (pos, pos + ((1 << 4) - 1), moves, P);
			}
		}
	} else {
		// move
		p = pos - (1 << 4);
		if (!(p & 0b10001000) && !board(p)) {
			if ((p >> 4) == 0) {
				add_move (pos, p | (QUEEN << 8), moves);
				add_move (pos, p | (KNIGHT << 8), moves);
				add_move (pos, p | (ROOK << 8), moves);
				add_move (pos, p | (BISHOP << 8), moves);
			} else {
				add_move (pos, p, moves);
				p -= (1 << 4);
				if ((pos >> 4) == 6 && !board(p)) {
					add_move (pos, p, moves);
				}
			}
		}

		// take
		p = pos + (-(1 << 4) | (1 & 0xf));
		if (!(p & 0b10001000)) {
			P = board (p);
			if (P && P->get_owner () == WHITE) {
				if ((p >> 4) == 0) {
					add_move (pos, p | (QUEEN << 8), moves, P);
					add_move (pos, p | (KNIGHT << 8), moves, P);
					add_move (pos, p | (ROOK << 8), moves, P);
					add_move (pos, p | (BISHOP << 8), moves, P);
				} else {
					add_move (pos, p, moves, P);
				}
			}
		}
		p = pos + (-(1 << 4) - (1 & 0xf));
		if (!(p & 0b10001000)) {
			P = board (p);
			if (P && P->get_owner () == WHITE) {
				if ((p >> 4) == 0) {
					add_move (pos, p | (QUEEN << 8), moves, P);
					add_move (pos, p | (KNIGHT << 8), moves, P);
					add_move (pos, p | (ROOK << 8), moves, P);
					add_move (pos, p | (BISHOP << 8), moves, P);
				} else {
					add_move (pos, p, moves, P);
				}
			}
		}

		// en passant
		if (last_moved == pos + 1) {
			P = board(last_moved);
			if (P->get_owner () == WHITE && P->can_be_en_passant ()) {
				add_move (pos, pos + (-(1 << 4) | 1), moves, P);
			}
		} else if (last_moved == pos - 1) {
			P = board(last_moved);
			if (P->get_owner () == WHITE && P->can_be_en_passant ()) {
				add_move (pos, pos + (-(1 << 4) - 1), moves, P);
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
	p = pos + (1 << 4);
	while (!(p & 0b10001000)) {
		P = board(p);
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, moves, P);
			}
			break;
		}
		add_move (pos, p, moves);
		p += (1 << 4);
	}

	// right
	p = pos + (1 & 0xf);
	while (!(p & 0b10001000)) {
		P = board(p);
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, moves, P);
			}
			break;
		}
		add_move (pos, p, moves);
		p += (1 & 0xf);
	}

	// down
	p = pos - (1 << 4);
	while (!(p & 0b10001000)) {
		P = board(p);
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, moves, P);
			}
			break;
		}
		add_move (pos, p, moves);
		p -= (1 << 4);
	}

	// left
	p = pos - (1 & 0xf);
	while (!(p & 0b10001000)) {
		P = board(p);
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, moves, P);
			}
			break;
		}
		add_move (pos, p, moves);
		p -= (1 & 0xf);
	}
}

bool Game::checked_diagonal (int pos) {
	int p;
	Piece* P;

	// rigth-up
	p = pos + ((1 << 4) | (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P) {
			if (P->get_owner () != turn) {
				if (P->get_type () & (BISHOP | QUEEN | KING)) {
					return true;
				} else if (turn == WHITE && P->get_type () == PAWN) {
					return true;
				}
			}
		} else {
			p += (1 << 4) | (1 & 0xf);
			while (!(p & 0b10001000)) {
				P = board(p);
				if (P) {
					if (P->get_owner () != turn && (P->get_type () & (BISHOP | QUEEN))) {
						return true;
					}
					break;
				}
				p += (1 << 4) | (1 & 0xf);
			}
		}
	}

	// right-down
	p = pos + (-(1 << 4) | (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P) {
			if (P->get_owner () != turn) {
				if (P->get_type () & (BISHOP | QUEEN | KING)) {
					return true;
				} else if (turn == BLACK && P->get_type () == PAWN) {
					return true;
				}
			}
		} else {
			p += -(1 << 4) | (1 & 0xf);
			while (!(p & 0b10001000)) {
				P = board(p);
				if (P) {
					if (P->get_owner () != turn && (P->get_type () & (BISHOP | QUEEN))) {
						return true;
					}
					break;
				}
				p += -(1 << 4) | (1 & 0xf);
			}
		}
	}

	// left-down
	p = pos + (-(1 << 4) - (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P) {
			if (P->get_owner () != turn) {
				if (P->get_type () & (BISHOP | QUEEN | KING)) {
					return true;
				} else if (turn == BLACK && P->get_type () == PAWN) {
					return true;
				}
			}
		} else {
			p += -(1 << 4) - (1 & 0xf);
			while (!(p & 0b10001000)) {
				P = board(p);
				if (P) {
					if (P->get_owner () != turn && (P->get_type () & (BISHOP | QUEEN))) {
						return true;
					}
					break;
				}
				p += -(1 << 4) - (1 & 0xf);
			}
		}
	}

	// left-up
	p = pos + ((1 << 4) - (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P) {
			if (P->get_owner () != turn) {
				if (P->get_type () & (BISHOP | QUEEN | KING)) {
					return true;
				} else if (turn == WHITE && P->get_type () == PAWN) {
					return true;
				}
			}
		} else {
			p += (1 << 4) - (1 & 0xf);
			while (!(p & 0b10001000)) {
				P = board(p);
				if (P) {
					if (P->get_owner () != turn && (P->get_type () & (BISHOP | QUEEN))) {
						return true;
					}
					break;
				}
				p += (1 << 4) - (1 & 0xf);
			}
		}
	}

	return false;
}
bool Game::checked_horizontal (int pos) {
	int p;
	Piece* P;

	// up
	p = pos + (1 << 4);
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P) {
			if (P->get_owner () != turn && P->get_type () & (ROOK | QUEEN | KING)) {
				return true;
			}
		} else {
			p += (1 << 4);
			while (!(p & 0b10001000)) {
				P = board(p);
				if (P) {
					if (P->get_owner () != turn && (P->get_type () & (ROOK | QUEEN))) {
						return true;
					}
					break;
				}
				p += (1 << 4);
			}
		}
	}

	// right
	p = pos + (1 & 0xf);
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P) {
			if (P->get_owner () != turn && P->get_type () & (ROOK | QUEEN | KING)) {
				return true;
			}
		} else {
			p += (1 & 0xf);
			while (!(p & 0b10001000)) {
				P = board(p);
				if (P) {
					if (P->get_owner () != turn && (P->get_type () & (ROOK | QUEEN))) {
						return true;
					}
					break;
				}
				p += (1 & 0xf);
			}
		}
	}

	// down
	p = pos - (1 << 4);
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P) {
			if (P->get_owner () != turn && P->get_type () & (ROOK | QUEEN | KING)) {
				return true;
			}
		} else {
			p -= (1 << 4);
			while (!(p & 0b10001000)) {
				P = board(p);
				if (P) {
					if (P->get_owner () != turn && (P->get_type () & (ROOK | QUEEN))) {
						return true;
					}
					break;
				}
				p -= (1 << 4);
			}
		}
	}

	// left
	p = pos - (1 & 0xf);
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P) {
			if (P->get_owner () != turn && P->get_type () & (ROOK | QUEEN | KING)) {
				return true;
			}
		} else {
			p -= (1 & 0xf);
			while (!(p & 0b10001000)) {
				P = board(p);
				if (P) {
					if (P->get_owner() != turn && (P->get_type() & (ROOK | QUEEN))) {
						return true;
					}
					break;
				}
				p -= (1 & 0xf);
			}
		}
	}

	return false;
}
bool Game::checked_by_knight (int pos) {
	int p;
	Piece* P;

	// up-right
	p = pos + ((2 << 4) | (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P && P->get_owner() != turn && P->get_type() == KNIGHT) {
			return true;
		}
	}

	// right-up
	p = pos + ((1 << 4) | (2 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P && P->get_owner() != turn && P->get_type() == KNIGHT) {
			return true;
		}
	}

	// right-down
	p = pos + (-(1 << 4) | (2 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P && P->get_owner() != turn && P->get_type() == KNIGHT) {
			return true;
		}
	}

	// down-right
	p = pos + (-(2 << 4) | (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P && P->get_owner() != turn && P->get_type() == KNIGHT) {
			return true;
		}
	}

	// down-left
	p = pos + (-(2 << 4) - (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P && P->get_owner() != turn && P->get_type() == KNIGHT) {
			return true;
		}
	}

	// left-down
	p = pos + (-(1 << 4) - (2 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P && P->get_owner() != turn && P->get_type() == KNIGHT) {
			return true;
		}
	}

	// left-up
	p = pos + ((1 << 4) - (2 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P && P->get_owner() != turn && P->get_type() == KNIGHT) {
			return true;
		}
	}

	// up-left
	p = pos + ((2 << 4) - (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P && P->get_owner() != turn && P->get_type() == KNIGHT) {
			return true;
		}
	}

	return false;
}

const char pieces_repr[33] = {'.', 
	'B', 
	'K', '.', 
	'k', '.', '.', '.', 
	'P', '.', '.', '.', '.', '.', '.', '.', 
	'Q', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', 
	'R'
};
void Game::print_board () {
	for (int i = 7; i >= 0; i--) {
		cout << "\t" << (i + 1) << "  |";
		for (int j = 0; j < 8; j++) {
			cout << "\t";
			if (board[(i << 3) | (j & 0x7)]) {
				cout << pieces_repr[board[(i << 3) | (j & 0x7)]->get_type ()];
				cout << (board[(i << 3) | (j & 0x7)]->get_owner () == WHITE ? 'W' : 'B');
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
			int from = move & 0xff;
			int to = move >> 8;
			cout << pieces_repr[board(from)->get_type ()] << "\t";
			cout << (char)((from & 0xf) + 'a') << ((from >> 4) + 1) << "\t->\t";
			cout << (char)((to & 0xf) + 'a') << (((to >> 4) & 0xf) + 1);
			if (to >> 8) {
				cout << pieces_repr[to >> 8];
			}
			cout << endl;
		}
	}
}
void Game::human_move (string move) {	// "a1 h8x" or "A"
	if (move[0] == 'A') {
		int best_move;
		int score = get_best_move (&best_move);
		int from = best_move & 0xff;
		int to = best_move >> 8;

		cout << "Predicted score: " << score << endl;
		cout << pieces_repr[board(from)->get_type ()] << "\t";
		cout << (char)((from & 0xf) + 'a') << ((from >> 4) + 1) << "\t->\t";
		cout << (char)((to & 0xf) + 'a') << (((to >> 4) & 0xf) + 1);
		if (to >> 8) {
			cout << pieces_repr[to >> 8];
		}
		cout << endl << endl;

		return;
	}
	int from_col = move[0] - 'a';
	int from_row = move[1] - '1';
	int to_col = move[3] - 'a';
	int to_row = move[4] - '1';
	int new_type = 0;
	if (move[5]) {
		for (new_type = 1; new_type < 34; new_type++) {
			if (move[5] == pieces_repr[new_type]) {
				break;
			}
		}
	}

	move_piece ((from_row << 4) | (from_col & 0xf), (to_row << 4) | (to_col & 0xf) | (new_type << 8));
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
		moved = board(moves[i] & 0xff);
		was_first_double_move = moved->can_be_en_passant ();
		had_moved = moved->is_first_move ();
		previous_type = moved->get_type ();
		eaten = simulate_move (moves[i]);
		if (eaten) {
			actual_score = -values[eaten->get_type ()];
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
		moved = board(moves[i] & 0xff);
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
		*best_move = my_best_move;
	}
	return best_score;
}
Piece* Game::simulate_move (int move) {
	int from = move & 0xff;
	int to = move >> 8;
	moves_without_take_or_pawn_move++;
	turn = !turn;
	Piece* eaten = nullptr;

	if (board(from)->get_type () == KING) {
		// short-castle
		if (to == from + 2) {
			board(from)->move (to);
			board(to) = board(from);
			board(from) = nullptr;
			board(from + 3)->move (from + 1);
			board(from + 1) = board(from + 3);
			board(from + 3) = nullptr;
			return nullptr;
		} else 
		// long castle
		if (to == from - 2) {
			board(from)->move (to);
			board(to) = board(from);
			board(from) = nullptr;
			board(from - 4)->move (from - 1);
			board(from - 1) = board(from - 4);
			board(from - 4) = nullptr;
			return nullptr;
		}
	}

	// en passant
	if (board(from)->get_type () == PAWN && (from & 0x7) != (to & 0x7) && !board(to & 0xff)) {
		eaten = board[((from >> 1) & 0b111000) | (to & 0b000111)];
		board[((from >> 1) & 0b111000) | (to & 0b000111)] = nullptr;
		remaining_pieces[turn]--;
	} else 
	// promotion
	if (to >> 8) {
		board(from)->promote (to >> 8);
	}

	to &= 0xff;
	last_moved = to;

	if (board(from)->get_type () == PAWN) {
		moves_without_take_or_pawn_move = 0;
	}

	if (board(to)) {
		eaten = board(to);
		moves_without_take_or_pawn_move = 0;
		remaining_pieces[turn]--;
		check_draw ();
	}
	board(from)->move (to);
	board(to) = board(from);
	board(from) = nullptr;

	if (moves_without_take_or_pawn_move == 100) {
		is_draw = true;
	}

	return eaten;
}
void Game::undo_simulated_move (int move, Piece* eaten) {
	int from = move & 0xff;
	int to = move >> 8;

	if (board(to & 0xff)->get_type () == KING) {
		// short-castle
		if (to == from + 2) {
			board(to)->move (from);
			board(from) = board(to);
			board(to) = nullptr;
			board(from + 1)->move (from + 3);
			board(from + 3) = board(from + 1);
			board(from + 1) = nullptr;
			board(from + 3)->undo_first_move ();
			is_checkmate = false;
			is_draw = false;
			turn = !turn;
			return;
		} else 
		// long castle
		if (to == from - 2) {
			board(to)->move (from);
			board(from) = board(to);
			board(to) = nullptr;
			board(from - 1)->move (from - 4);
			board(from - 4) = board(from - 1);
			board(from - 1) = nullptr;
			board(from - 4)->undo_first_move ();
			is_checkmate = false;
			is_draw = false;
			turn = !turn;
			return;
		}
	}

	// promotion
	if (to >> 8) {
		board(to & 0xff)->undo_promote ();
	}

	to &= 0xff;

	board(to)->move (from);
	board(from) = board(to);
	board(to) = nullptr;

	if (eaten) {
		remaining_pieces[turn]++;
		board(eaten->get_pos ()) = eaten;
	}

	turn = !turn;
	is_checkmate = false;
	is_draw = false;
}