#pragma once

#define BISHOP	(1 << 0)
#define KING	(1 << 1)
#define KNIGHT	(1 << 2)
#define PAWN	(1 << 3)
#define QUEEN	(1 << 4)
#define ROOK	(1 << 5)

class Piece {
private:
	int pos;
	bool has_moved;	// for pawn first move and castle
	int type;
	int owner;
	bool first_double_move; // for en passant

public:
	Piece (int pos_, int owner_, int type_) {
		has_moved = false;
		pos = pos_;
		owner = owner_;
		type = type_;
		first_double_move = false;
	}
	inline int get_pos () {
		return pos;
	}

	inline void set_pos (int pos_) {
		pos = pos_;
	}

	inline int get_type () {
		return type;
	}

	inline int get_owner () {
		return owner;
	}

	inline bool is_first_move () {
		return !has_moved;
	}

	inline bool can_be_en_passant () {
		return first_double_move;
	}

	inline void promote (int type_) {
		type = type_;
	}

	inline void undo_promote () {
		type = PAWN;
	}

	void move (int to) {
		pos = to;
		first_double_move = false;
		if (type == PAWN && to != pos + (1 << 4) && to != pos - (1 << 4)) {
			first_double_move = true;
		}
		has_moved = true;
	}
};