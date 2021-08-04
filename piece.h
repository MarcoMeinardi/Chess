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
	Piece (int pos_, int owner_, int type_);
	int get_pos ();
	int get_type ();
	int get_owner ();
	bool is_first_move ();
	bool can_be_en_passant ();

	void move (int to);
};