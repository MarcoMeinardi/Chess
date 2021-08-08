import pygame

black       = (0x7c, 0x55, 0x36)
white       = (0xe0, 0xc0, 0x91)
background  = (0x42, 0x29, 0x1a)


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

def draw_board ():
	screen.fill (background)

	for i in range (8):
		for j in range (8):
			if (i + j) % 2 == 0:
				pygame.draw.rect (screen, white, (100 * (j + 1), 100 * (i + 1), 100, 100))
			else:
				pygame.draw.rect (screen, black, (100 * (j + 1), 100 * (i + 1), 100, 100))
			if board[7 - i][j]:
				screen.blit (pieces[board[7 - i][j]], (100 * (j + 1), 100 * (i + 1), 100, 100))

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



running = True

while running:
	for event in pygame.event.get ():
		if event.type == pygame.QUIT:
			running = False

	screen.fill (background)

	draw_board ()


	pygame.display.flip ()

pygame.quit ()