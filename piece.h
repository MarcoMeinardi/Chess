#pragma once

#define BISHOP	(1 << 0)
#define KING	(1 << 1)
#define KNIGHT	(1 << 2)
#define PAWN	(1 << 3)
#define QUEEN	(1 << 4)
#define ROOK	(1 << 5)

#define Y(pos) (pos >> 3)
#define X(pos) (pos & 0x07)

class Piece {
private:
	int pos;
	bool has_moved;	// for castle
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

	int get_pos () {
		return pos;
	}

	void set_pos (int pos_) {
		pos = pos_;
	}

	int get_type () {
		return type;
	}

	int get_owner () {
		return owner;
	}

	bool is_first_move () {
		return !has_moved;
	}

	bool can_be_en_passant () {
		return first_double_move;
	}

	void promote (int type_) {
		type = type_;
	}

	void undo_promote () {
		type = PAWN;
	}

	void undo_first_move () {
		has_moved = false;
	}

	void set_prev_first_double_move (bool first_double_move_) {
		first_double_move = first_double_move_;
	}

	void move (int to) {
		has_moved = true;
		first_double_move = false;
		if (type == PAWN && Y (to) != Y (pos) + 1 && Y (to) != Y (pos) - 1) {
			first_double_move = true;
		}
		pos = to;
	}
};