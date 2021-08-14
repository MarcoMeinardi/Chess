from subprocess import Popen, PIPE

r = Popen (["../chess"], stdin = PIPE, stdout = PIPE, encoding = "utf-8", bufsize = 1)


BISHOP	= 1 << 0
KING	= 1 << 1
KNIGHT	= 1 << 2
PAWN	= 1 << 3
QUEEN	= 1 << 4
ROOK	= 1 << 5

WHITE	= 0
BLACK	= 1

COORD = lambda yx: (yx[0] << 3) | (yx[1] & 0x7)
SPLIT_COORD = lambda pos: [pos >> 3, pos & 0x7]
GET_PIECE = lambda piece: piece & 0b11111

promote = [QUEEN, ROOK, KNIGHT, BISHOP]
computer_promotion = {
	QUEEN:	0,
	ROOK:	1,
	KNIGHT:	2,
	BISHOP:	3
}

def get_possible_moves (y, x):
	pos = COORD ([y, x])
	r.stdin.write ("S\n")
	r.stdin.write (str (pos) + "\n")
	n_moves = int (r.stdout.readline ()[:-1])
	moves = set ()
	for i in range (n_moves):
		move = int (r.stdout.readline ()[:-1])
		moves.add (move)
	return moves

def move_piece_remote (fr, to, promotion):
	fr = COORD (fr)
	to = COORD (to)
	r.stdin.write ("M\n")
	if promotion is not None:
		r.stdin.write (f"{fr} {to | (promote[promotion] << 8)}\n")
	else:
		r.stdin.write (f"{fr} {to}\n")

	resp = r.stdout.readline ()[:-1]
	if resp == "O":  # castle
		return resp
	resp = resp.split ()
	eaten = int (resp[0])
	if len (resp) != 1:
		if resp[1] == "D":
			print ("Draw")
		else:
			print ("Checkmate")
			if resp[1][1] == "W":
				print ("White won")
			else:
				print ("Black won")
			
	if eaten == -1:
		return None
	else:
		return SPLIT_COORD (eaten)

def auto_move ():
	r.stdin.write ("A\n")
	fr = SPLIT_COORD (int (r.stdout.readline()[:-1]))
	to = int (r.stdout.readline()[:-1])
	if to >> 8:
		promotion = to >> 8
		to &= 0xff
	else:
		promotion = None
	to = SPLIT_COORD (to)

	resp = r.stdout.readline ()[:-1]
	if resp == "O":  # castle
		return (fr, to, resp)

	resp = resp.split ()
	eaten = int (resp[0])
	if len (resp) != 1:
		if resp[1] == "D":
			print ("Draw")
		else:
			print ("Checkmate")
			if resp[1][1] == "W":
				print ("White won")
			else:
				print ("Black won")
			
	if eaten == -1:
		return (fr, to, None, computer_promotion.get (promotion, None))
	else:
		return (fr, to, SPLIT_COORD (eaten), computer_promotion.get (promotion, None))
