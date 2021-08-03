#pragma once

#define BISHOP	0
#define KING	1
#define KNIGHT	2
#define PAWN	3
#define QUEEN	4
#define ROOK	5

#define WHITE 0
#define BLACK 1


class Piece {
private:
	int y, x;
	bool has_moved;	// for pawn first move and castle
	int type;
	int owner;
	bool just_moved; // for en passant

public:
	Piece (int y_, int x_, int owner_, int type_);
	int get_type ();
	int get_owner ();

	void move (int y_, int x_);

	void set_has_just_moved (bool actual);
	bool has_just_moved ();

};