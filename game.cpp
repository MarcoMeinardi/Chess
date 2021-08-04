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

	is_checkmate = false;
	is_draw = false;

	kings[WHITE] = board [(0 << 3) | (4 & 0x7)];
	kings[BLACK] = board [(7 << 3) | (4 & 0x7)];
}

void Game::move_piece (int from, int to) {
	turn ^= 1;
	last_moved = to & 0xff;

	if (board(from)->get_type () == KING) {
		// short-castle
		if (to == from + 2) {
			board(from)->move (to & 0xff);
			board(to & 0xff) = board(from);
			board(from) = nullptr;
			board(from + 3)->move (from + 1);
			board(from + 1) = board(from + 3);
			board(from + 3) = nullptr;
			return;
		} else 
		// long castle
		if (to == from - 2) {
			board(from)->move (to & 0xff);
			board(to & 0xff) = board(from);
			board(from) = nullptr;
			board(from - 4)->move (from - 1);
			board(from - 1) = board(from - 4);
			board(from - 4) = nullptr;
			return;
		}
	}

	// en passant
	if (board(from)->get_type () == PAWN && (from & 0x7) != (to & 0x7) && !board(to & 0xff)) {
		free (board[(from >> 4) | (to & 0x7)]);
	}

	if (to >> 8) {
		board(from)->promote (to >> 8);
	}

	if (board(to & 0xff)) {
		free (board(to & 0xff));
	}
	board(from)->move (to & 0xff);
	board(to & 0xff) = board(from);
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

	if (n_moves == 0) {
		if (is_check (kings[turn]->get_pos ())) {
			is_checkmate = true;
		} else {
			is_draw = true;
		}
	}
}
void Game::add_move (int from, int to, Piece* eaten = nullptr) {
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
		moves[n_moves++] = from | (to << 8);
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
	if (checked_by_bishop_queen (pos)) return true;
	if (checked_by_rook_queen (pos)) return true;
	if (checked_by_knight (pos)) return true;
	if (checked_near (pos)) return true;
	if (checked_by_pawn (pos)) return true;

	return false;
}

void Game::get_bishop_moves (int pos) {
	int p;
	Piece* P;

	// rigth-up
	p = pos + ((1 << 4) | (1 & 0xf));
	while (!(p & 0b10001000)) {
		P = board(p);
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, P);
			}
			break;
		}
		add_move (pos, p);
		p += (1 << 4) | (1 & 0xf);
	}

	// right-down
	p = pos + (-(1 << 4) | (1 & 0xf));
	while (!(p & 0b10001000)) {
		P = board (p);
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, P);
			}
			break;
		}
		add_move (pos, p);
		p += -(1 << 4) | (1 & 0xf);
	}

	// left-down
	p = pos + (-(1 << 4) - (1 & 0xf));
	while (!(p & 0b10001000)) {
		P = board (p);
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, P);
			}
			break;
		}
		add_move (pos, p);
		p += -(1 << 4) - (1 & 0xf);
	}

	// left-up
	p = pos + ((1 << 4) - (1 & 0xf));
	while (!(p & 0b10001000)) {
		P = board(p);
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, P);
			}
			break;
		}
		add_move (pos, p);
		p += (1 << 4) - (1 & 0xf);
	}
}
void Game::get_king_moves (int pos) {
	int p;
	Piece* P;

	// up
	p = pos + (1 << 4);
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, P);
		}
	}

	// up-right
	p = pos + ((1 << 4) | (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, P);
		}
	}

	// right
	p = pos + 1;
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, P);
		}
	}

	// down-right
	p = pos + (-(1 << 4) | (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, P);
		}
	}

	// down
	p = pos - (1 << 4);
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, P);
		}
	}

	// down-left
	p = pos + (-(1 << 4) - (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, P);
		}
	}

	// left
	p = pos - 1;
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, P);
		}
	}

	// left-up
	p = pos + ((1 << 4) - (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, P);
		}
	}

	// short-castle
	if (board(pos)->is_first_move () && !board(pos + 1) && !board(pos + 2)) {
		P = board(pos + 3);
		if (P && P->get_type () == ROOK && P->is_first_move()) {
			if (
				!is_check (pos) && 
				!is_check (pos + 1) && 
				!is_check (pos + 3)
			) {
				add_move (pos, pos + 2);
			}
		}
	}
	// long-castle
	if (board(pos)->is_first_move () && !board(pos - 1) && !board(pos - 2) && !board(pos - 3)) {
		P = board(pos - 4);
		if (P && P->get_type () == ROOK && P->is_first_move()) {
			if (
				!is_check (pos) && 
				!is_check (pos - 1) &&
				!is_check (pos - 3) && 
				!is_check (pos - 4)
			) {
				add_move (pos, pos - 2);
			}
		}
	}
}
void Game::get_knight_moves (int pos) {
	int p;
	Piece* P;

	// up-right
	p = pos + ((2 << 4) | (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, P);
		}
	}

	// right-up
	p = pos + ((1 << 4) | (2 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, P);
		}
	}

	// right-down
	p = pos + (-(1 << 4) | (2 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, P);
		}
	}

	// down-right
	p = pos + (-(2 << 4) | (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, P);
		}
	}

	// down-left
	p = pos + (-(2 << 4) - (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, P);
		}
	}

	// left-down
	p = pos + (-(1 << 4) - (2 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, P);
		}
	}

	// left-up
	p = pos + ((1 << 4) - (2 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, P);
		}
	}

	// up-left
	p = pos + ((2 << 4) - (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (!P || P->get_owner () != turn) {
			add_move (pos, p, P);
		}
	}
}
void Game::get_pawn_moves (int pos) {
	int p;
	Piece* P;
	if (turn == WHITE) {
		// move
		p = pos + (1 << 4);
		if (!(p & 0b10001000) && !board(p)) {
			if ((p >> 4) == 7) {
				add_move (pos, p | (QUEEN << 8));
				add_move (pos, p | (KNIGHT << 8));
				add_move (pos, p | (ROOK << 8));
				add_move (pos, p | (BISHOP << 8));
			} else {
				add_move (pos, p);
				p += (1 << 4);
				if (board(pos)->is_first_move () && !(p & 0b10001000) && !board(p)) {
					add_move (pos, p);	
				}
			}
		}

		// take
		p = pos + ((1 << 4) | (1 & 0xf));
		if (!(p & 0b10001000)) {
			P = board(p);
			if (P && P->get_owner () == BLACK) {
				if ((p >> 4) == 7) {
					add_move (pos, p | (QUEEN << 8), P);
					add_move (pos, p | (KNIGHT << 8), P);
					add_move (pos, p | (ROOK << 8), P);
					add_move (pos, p | (BISHOP << 8), P);
				} else {
					add_move (pos, p, P);
				}
			}
		}
		p = pos + ((1 << 4) - (1 & 0xf));
		if (!(p & 0b10001000)) {
			P = board(p);
			if (P && P->get_owner () == BLACK) {
				if ((p >> 4) == 7) {
					add_move (pos, p | (QUEEN << 8), P);
					add_move (pos, p | (KNIGHT << 8), P);
					add_move (pos, p | (ROOK << 8), P);
					add_move (pos, p | (BISHOP << 8), P);
				} else {
					add_move (pos, p, P);
				}
			}
		}

		// en passant
		if (last_moved == pos + 1) {
			P = board(last_moved);
			if (P->can_be_en_passant ()) {
				add_move (pos, pos + ((1 << 4) | 1), P);
			}
		} else if (last_moved == pos - 1) {
			P = board(last_moved);
			if (P->can_be_en_passant ()) {
				add_move (pos, pos + ((1 << 4) - 1), P);
			}
		}
	} else {
		// move
		p = pos - (1 << 4);
		if (!(p & 0b10001000) && !board(p)) {
			if ((p >> 4) == 0) {
				add_move (pos, p | (QUEEN << 8));
				add_move (pos, p | (KNIGHT << 8));
				add_move (pos, p | (ROOK << 8));
				add_move (pos, p | (BISHOP << 8));
			} else {
				add_move (pos, p);
				p -= (1 << 4);
				if (board(pos)->is_first_move () && !(p & 0b10001000) && !board(p)) {
					add_move (pos, p);
				}
			}
		}

		// take
		p = pos + (-(1 << 4) | (1 & 0xf));
		if (!(p & 0b10001000)) {
			P = board (p);
			if (P && P->get_owner () == WHITE) {
				if ((p >> 4) == 0) {
					add_move (pos, p | (QUEEN << 8), P);
					add_move (pos, p | (KNIGHT << 8), P);
					add_move (pos, p | (ROOK << 8), P);
					add_move (pos, p | (BISHOP << 8), P);
				} else {
					add_move (pos, p, P);
				}
			}
		}
		p = pos + (-(1 << 4) - (1 & 0xf));
		if (!(p & 0b10001000)) {
			P = board (p);
			if (P && P->get_owner () == WHITE) {
				if ((p >> 4) == 0) {
					add_move (pos, p | (QUEEN << 8), P);
					add_move (pos, p | (KNIGHT << 8), P);
					add_move (pos, p | (ROOK << 8), P);
					add_move (pos, p | (BISHOP << 8), P);
				} else {
					add_move (pos, p, P);
				}
			}
		}

		// en passant
		if (last_moved == pos + 1) {
			P = board(last_moved);
			if (P->can_be_en_passant ()) {
				add_move (pos, pos + (-(1 << 4) | 1), P);
			}
		} else if (last_moved == pos - 1) {
			P = board(last_moved);
			if (P->can_be_en_passant ()) {
				add_move (pos, pos + (-(1 << 4) - 1), P);
			}
		}
	}
}
void Game::get_queen_moves (int pos) {
	get_bishop_moves (pos);
	get_rook_moves (pos);
}
void Game::get_rook_moves (int pos) {
	int p;
	Piece* P;

	// up
	p = pos + (1 << 4);
	while (!(p & 0b10001000)) {
		P = board(p);
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, P);
			}
			break;
		}
		add_move (pos, p);
		p += (1 << 4);
	}

	// right
	p = pos + (1 & 0xf);
	while (!(p & 0b10001000)) {
		P = board(p);
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, P);
			}
			break;
		}
		add_move (pos, p);
		p += (1 & 0xf);
	}

	// down
	p = pos - (1 << 4);
	while (!(p & 0b10001000)) {
		P = board(p);
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, P);
			}
			break;
		}
		add_move (pos, p);
		p -= (1 << 4);
	}

	// left
	p = pos - (1 & 0xf);
	while (!(p & 0b10001000)) {
		P = board(p);
		if (P) {
			if (P->get_owner () != turn) {
				add_move (pos, p, P);
			}
			break;
		}
		add_move (pos, p);
		p -= (1 & 0xf);
	}
}

bool Game::checked_by_bishop_queen (int pos) {
	int p;
	Piece* P;

	// rigth-up
	p = pos + ((1 << 4) | (1 & 0xf));
	if (!(p & 0b10001000) && !board(p)) {
		p += (1 << 4) | (1 & 0xf);
		while (!(p & 0b10001000)) {
			P = board(p);
			if (P) {
				if (P->get_owner() != turn && (P->get_type() & (BISHOP | QUEEN))) {
					return true;
				}
				break;
			}
			p += (1 << 4) | (1 & 0xf);
		}
	}

	// right-down
	p = pos + (-(1 << 4) | (1 & 0xf));
	if (!(p & 0b10001000) && !board(p)) {
		p += -(1 << 4) | (1 & 0xf);
			while (!(p & 0b10001000)) {
			P = board(p);
			if (P) {
				if (P->get_owner() != turn && (P->get_type() & (BISHOP | QUEEN))) {
					return true;
				}
				break;
			}
			p += -(1 << 4) | (1 & 0xf);
		}
	}

	// left-down
	p = pos + (-(1 << 4) - (1 & 0xf));
	if (!(p & 0b10001000) && !board(p)) {
		p += -(1 << 4) - (1 & 0xf);
		while (!(p & 0b10001000)) {
			P = board(p);
			if (P) {
				if (P->get_owner() != turn && (P->get_type() & (BISHOP | QUEEN))) {
					return true;
				}
				break;
			}
			p += -(1 << 4) - (1 & 0xf);
		}
	}

	// left-up
	p = pos + ((1 << 4) - (1 & 0xf));
	if (!(p & 0b10001000) && !board(p)) {
		p += (1 << 4) - (1 & 0xf);
		while (!(p & 0b10001000)) {
			P = board(p);
			if (P) {
				if (P->get_owner() != turn && (P->get_type() & (BISHOP | QUEEN))) {
					return true;
				}
				break;
			}
			p += (1 << 4) - (1 & 0xf);
		}
	}

	return false;
}
bool Game::checked_by_rook_queen (int pos) {
	int p;
	Piece* P;

	// up
	p = pos + (1 << 4);
	if (!(p & 0b10001000) && !board(p)) {
		p += (1 << 4);
		while (!(p & 0b10001000)) {
			P = board(p);
			if (P) {
				if (P->get_owner() != turn && (P->get_type() & (ROOK | QUEEN))) {
					return true;
				}
				break;
			}
			p += (1 << 4);
		}
	}

	// right
	p = pos + (1 & 0xf);
	if (!(p & 0b10001000) && !board(p)) {
		p += (1 & 0xf);
		while (!(p & 0b10001000)) {
			P = board(p);
			if (P) {
				if (P->get_owner() != turn && (P->get_type() & (ROOK | QUEEN))) {
					return true;
				}
				break;
			}
			p += (1 & 0xf);
		}
	}

	// down
	p = pos - (1 << 4);
	if (!(p & 0b10001000) && !board(p)) {
		p -= (1 << 4);
		while (!(p & 0b10001000)) {
			P = board(p);
			if (P) {
				if (P->get_owner() != turn && (P->get_type() & (ROOK | QUEEN))) {
					return true;
				}
				break;
			}
			p -= (1 << 4);
		}
	}

	// left
	p = pos - (1 & 0xf);
	if (!(p & 0b10001000) && !board(p)) {
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
bool Game::checked_near (int pos) {
	int p;
	Piece* P;

	// up
	p = pos + (1 << 4);
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P && P->get_owner() != turn && (P->get_type() & (KING | ROOK | QUEEN))) {
			return true;
		}
	}

	// right-up
	p = pos + ((1 << 4) | (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P && P->get_owner() != turn && (P->get_type() & (KING | BISHOP | QUEEN))) {
			return true;
		}
	}

	// right
	p = pos + 1;
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P && P->get_owner() != turn && (P->get_type() & (KING | ROOK | QUEEN))) {
			return true;
		}
	}

	// right-down
	p = pos + (-(1 << 4) | (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P && P->get_owner() != turn && (P->get_type() & (KING | BISHOP | QUEEN))) {
			return true;
		}
	}

	// down
	p = pos - (1 << 4);
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P && P->get_owner() != turn && (P->get_type() & (KING | ROOK | QUEEN))) {
			return true;
		}
	}

	// left-down
	p = pos + (-(1 << 4) - (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P && P->get_owner() != turn && (P->get_type() & (KING | BISHOP | QUEEN))) {
			return true;
		}
	}

	// left
	p = pos - 1;
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P && P->get_owner() != turn && (P->get_type() & (KING | ROOK | QUEEN))) {
			return true;
		}
	}

	// left-up
	p = pos + ((1 << 4) - (1 & 0xf));
	if (!(p & 0b10001000)) {
		P = board(p);
		if (P && P->get_owner() != turn && (P->get_type() & (KING | BISHOP | QUEEN))) {
			return true;
		}
	}

	return false;
}
bool Game::checked_by_pawn (int pos) {
	int p;
	Piece* P;

	if (turn == WHITE) {
		p = pos + ((1 << 4) | (1 & 0xf));
		if (!(p & 0b10001000)) {
			P = board(p);
			if (P && P->get_owner() == BLACK && P->get_type() == PAWN) {
				return true;
			}
		}
		p = pos + ((1 << 4) - (1 & 0xf));
		if (!(p & 0b10001000)) {
			P = board(p);
			if (P && P->get_owner() == BLACK && P->get_type() == PAWN) {
				return true;
			}
		}
	} else {
		p = pos + (-(1 << 4) | (1 & 0xf));
		if (!(p & 0b10001000)) {
			P = board(p);
			if (P && P->get_owner() == WHITE && P->get_type() == PAWN) {
				return true;
			}
		}
		p = pos + (-(1 << 4) - (1 & 0xf));
		if (!(p & 0b10001000)) {
			P = board(p);
			if (P && P->get_owner() == WHITE && P->get_type() == PAWN) {
				return true;
			}
		}
	}
	return false;
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
void Game::human_move (string move) {	// "a1 h8x"
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