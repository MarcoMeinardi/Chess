#pragma once

class Piece {
private:
	int pos;
	bool has_moved;	// for pawn first move and castle
	int type;
	int owner;

public:
	Piece (int pos_, int owner_, int type_);
	int get_pos ();
	int get_type ();
	int get_owner ();

	void move (int to);
};