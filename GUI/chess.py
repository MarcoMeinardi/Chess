import pygame
from subprocess import Popen, PIPE

black			= (0x7c, 0x55, 0x36)
white			= (0xe0, 0xc0, 0x91)
background		= (0x42, 0x29, 0x1a)

selected_surface = pygame.Surface ((100, 100))
selected_surface.set_alpha (0x80)
selected_surface.fill ((0xff, 0xff, 0x00))

pygame.init ()
pygame.font.init ()
font = pygame.font.SysFont('Comic Sans MS', 30)

BISHOP	= 1 << 0
KING	= 1 << 1
KNIGHT	= 1 << 2
PAWN	= 1 << 3
QUEEN	= 1 << 4
ROOK	= 1 << 5

WHITE	= 0
BLACK	= 1

pieces = {
	BISHOP	| (WHITE << 6): pygame.image.load ("pieces/wb.png"),
	KING	| (WHITE << 6): pygame.image.load ("pieces/wk.png"),
	KNIGHT	| (WHITE << 6): pygame.image.load ("pieces/wn.png"),
	PAWN	| (WHITE << 6): pygame.image.load ("pieces/wp.png"),
	QUEEN	| (WHITE << 6): pygame.image.load ("pieces/wq.png"),
	ROOK	| (WHITE << 6): pygame.image.load ("pieces/wr.png"),
	BISHOP	| (BLACK << 6): pygame.image.load ("pieces/bb.png"),
	KING	| (BLACK << 6): pygame.image.load ("pieces/bk.png"),
	KNIGHT	| (BLACK << 6): pygame.image.load ("pieces/bn.png"),
	PAWN	| (BLACK << 6): pygame.image.load ("pieces/bp.png"),
	QUEEN	| (BLACK << 6): pygame.image.load ("pieces/bq.png"),
	ROOK	| (BLACK << 6): pygame.image.load ("pieces/br.png")
}

board = [
	[ROOK	| (WHITE << 6), KNIGHT	| (WHITE << 6), BISHOP	| (WHITE << 6), QUEEN	| (WHITE << 6), KING	| (WHITE << 6), BISHOP	| (WHITE << 6), KNIGHT	| (WHITE << 6), ROOK	| (WHITE << 6)],
	[PAWN	| (WHITE << 6), PAWN	| (WHITE << 6), PAWN	| (WHITE << 6), PAWN	| (WHITE << 6), PAWN	| (WHITE << 6), PAWN	| (WHITE << 6), PAWN	| (WHITE << 6), PAWN	| (WHITE << 6)],
	[0, 0, 0, 0, 0, 0, 0, 0],
	[0, 0, 0, 0, 0, 0, 0, 0],
	[0, 0, 0, 0, 0, 0, 0, 0],
	[0, 0, 0, 0, 0, 0, 0, 0],
	[PAWN	| (BLACK << 6), PAWN	| (BLACK << 6), PAWN	| (BLACK << 6), PAWN	| (BLACK << 6), PAWN	| (BLACK << 6), PAWN	| (BLACK << 6), PAWN	| (BLACK << 6), PAWN	| (BLACK << 6)],
	[ROOK	| (BLACK << 6), KNIGHT	| (BLACK << 6), BISHOP	| (BLACK << 6), QUEEN	| (BLACK << 6), KING	| (BLACK << 6), BISHOP	| (BLACK << 6), KNIGHT	| (BLACK << 6), ROOK	| (BLACK << 6)]
]


screen = pygame.display.set_mode ([1000, 1000])
r = Popen (["../chess"], stdin = PIPE, stdout = PIPE, encoding = "utf-8", bufsize = 0)
stdin  = r.stdin
stdout = r.stdout

selected = [-1, -1]
mouse_is_down = False

def draw_board (board, selected):
	screen.fill (background)

	for i in range (8)[::-1]:
		for j in range (8):
			rect = (100 * (j + 1), 100 * ((8 - i)), 100, 100)
			# tile
			if (i + j) % 2:
				pygame.draw.rect (screen, white, rect)
			else:
				pygame.draw.rect (screen, black, rect)

			if selected == [i, j]:
				# selected tile
				screen.blit (selected_surface, rect[:2])
			if (selected != [i, j] or not mouse_is_down) and board[i][j]:
				# piece
				screen.blit (pieces[board[i][j]], rect)

	# coord
	for i in range (8):
			if i % 2 == 0:
				screen.blit (font.render (chr (ord ('8') - i), False, black), (105, 100 * (i + 1) + 5))
			else:
				screen.blit (font.render (chr (ord ('8') - i), False, white), (105, 100 * (i + 1) + 5))
	for i in range (8):
		if i % 2 == 1:
			screen.blit (font.render (chr (ord ('a') + i), False, black), (100 * (i + 1) + 85, 875))
		else:
			screen.blit (font.render (chr (ord ('a') + i), False, white), (100 * (i + 1) + 85, 875))

	# selected piece
	if selected[0] != -1 and mouse_is_down and board[selected[0]][selected[1]]:
		x, y = pygame.mouse.get_pos ()
		screen.blit (pieces[board[selected[0]][selected[1]]], (x - 50, y - 50, 100, 100))

def handle_mouse_down ():
	global selected, mouse_is_down
	x, y = pygame.mouse.get_pos ()
	x = (x // 100) - 1
	y = 8 - (y // 100)
	if 0 <= x < 8 and 0 <= y < 8:
		selected = [ y, x]
	mouse_is_down = True
def handle_mouse_up ():
	global selected, mouse_is_down
	x, y = pygame.mouse.get_pos ()
	x = (x // 100) - 1
	y = 8 - (y // 100)
	if selected[0] != -1 and board[selected[0]][selected[1]]:
		if 0 <= x < 8 and 0 <= y < 8 and selected != [y, x]:
			board[y][x] = board[selected[0]][selected[1]]
			board[selected[0]][selected[1]] = 0
	if 0 <= x < 8 and 0 <= y < 8 and selected != [y, x]:
		selected = [-1, -1]
	mouse_is_down = False

running = True

while running:
	for event in pygame.event.get ():
		if event.type == pygame.QUIT:
			running = False

		elif event.type == pygame.MOUSEBUTTONDOWN:
			handle_mouse_down ()
		elif event.type == pygame.MOUSEBUTTONUP:
			handle_mouse_up ()

	screen.fill (background)

	draw_board (board, selected)


	pygame.display.flip ()

pygame.quit ()