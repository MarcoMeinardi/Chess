from subprocess import Popen, PIPE

r = Popen (["../chess"], stdin = PIPE, stdout = PIPE, encoding = "utf-8", bufsize = 0)
stdin  = r.stdin
stdout = r.stdout


COORD = lambda yx: (yx[0] << 3) | (yx[1] & 0x7)
SPLIT_COORD = lambda pos: (pos >> 3, pos & 0x7)

def get_possible_moves (y, x):
    pos = COORD ([y, x])
    stdin.write ("S\n")
    stdin.write (str (pos) + "\n")
    n_moves = int (stdout.readline ()[:-1])
    moves = set ()
    for i in range (n_moves):
        move = int (stdout.readline ()[:-1])
        moves.add (move)
    return moves

def move_piece_remote (fr, to):
    fr = COORD (fr)
    to = COORD (to)
    stdin.write ("M\n")
    stdin.write (f"{fr} {to}\n")
    resp = stdout.readline ()[:-1]
    if resp == "O":  # castle
        return resp
    eaten = int (resp)
    if eaten == -1:
        return None
    else:
        return SPLIT_COORD (eaten)