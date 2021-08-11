#!/bin/python3

import pygame
from communicator import *

black			= (0x7c, 0x55, 0x36)
white			= (0xe0, 0xc0, 0x91)
background		= (0x42, 0x29, 0x1a)

selected_surface = pygame.Surface ((100, 100), pygame.SRCALPHA)
selected_surface.set_alpha (0x80)
selected_surface.fill ((0xff, 0xff, 0x00))

edge_surface = pygame.Surface ((100, 100), pygame.SRCALPHA)
edge_surface.set_alpha (0xa0)
edge_surface.fill ((0xff, 0xff, 0xff))
pygame.draw.rect (edge_surface, (0x00, 0x00, 0x00, 0x00), (5, 5, 90, 90))

possible_move_surface = pygame.Surface ((100, 100), pygame.SRCALPHA)
pygame.draw.circle (possible_move_surface, (0x00, 0x00, 0x00, 0x30), (50, 50), 20)

possible_eat_surface = pygame.Surface ((100, 100), pygame.SRCALPHA)
pygame.draw.circle (possible_eat_surface, (0x00, 0x00, 0x00, 0x30), (50, 50), 48)
pygame.draw.circle (possible_eat_surface, (0x00, 0x00, 0x00, 0x00), (50, 50), 40)


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

selected = None
prev_pos = None
act_pos  = None
mouse_is_down = False
possible_moves = set ()

def draw_board ():
	global board, selected, possible_moves
	screen.fill (background)

	for i in range (8)[::-1]:
		for j in range (8):
			rect = (100 * (j + 1), 100 * ((8 - i)), 100, 100)
			# tile
			if (i + j) % 2:
				pygame.draw.rect (screen, white, rect)
			else:
				pygame.draw.rect (screen, black, rect)

			if selected == [i, j] or prev_pos == [i, j] or act_pos == [i, j]:
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
	if selected is not None and mouse_is_down and board[selected[0]][selected[1]]:
		x, y = pygame.mouse.get_pos ()
		grid_y = y // 100
		grid_x = x // 100
		if 0 < grid_y <= 8 and 0 < grid_x <= 8:
			screen.blit (edge_surface, (grid_x * 100, grid_y * 100, 100, 100))
		screen.blit (pieces[board[selected[0]][selected[1]]], (x - 50, y - 50, 100, 100))

	# possible_moves
	for move in possible_moves:
		y, x = SPLIT_COORD (move)
		print (y, x)
		if board[y][x]:
			screen.blit (possible_eat_surface, (100 * (x + 1), 100 * (8 - y), 100, 100))
		else:
			screen.blit (possible_move_surface, (100 * (x + 1), 100 * (8 - y), 100, 100))


def move_piece (fr, to):
	global board, prev_pos, act_pos, possible_moves
	if COORD (to) in possible_moves:
		eaten = move_piece_remote (fr, to)
		if eaten:
			if eaten == "O":
				if to[1] == fr[1] + 2:
					board[fr[0]][fr[1] + 1] = board[fr[0]][fr[1] + 3]
					board[fr[0]][fr[1] + 3] = 0
				else:
					board[fr[0]][fr[1] - 1] = board[fr[0]][fr[1] - 4]
					board[fr[0]][fr[1] - 4] = 0
			else:
				board[eaten[0]][eaten[1]] = 0
		board[to[0]][to[1]] = board[fr[0]][fr[1]]
		board[fr[0]][fr[1]] = 0
		prev_pos = fr
		act_pos = to
		return True
	else:
		return False

def handle_mouse_down ():
	global selected, mouse_is_down, possible_moves
	x, y = pygame.mouse.get_pos ()
	x = (x // 100) - 1
	y = 8 - (y // 100)
	if 0 <= x < 8 and 0 <= y < 8:
		if selected is not None:
			if selected != [y, x]:
				if move_piece (selected, [y, x]):
					selected = None
					possible_moves = set ()
				else:
					selected = None
					possible_moves = set ()
					handle_mouse_down ()
		elif board[y][x]:
			selected = [y, x]
			possible_moves = get_possible_moves (y, x)
	else:
		selected = None
		possible_moves = set ()
	mouse_is_down = True

def handle_mouse_up ():
	global selected, mouse_is_down, possible_moves
	x, y = pygame.mouse.get_pos ()
	x = (x // 100) - 1
	y = 8 - (y // 100)
	if selected is not None and board[selected[0]][selected[1]]:
		if 0 <= x < 8 and 0 <= y < 8 and selected != [y, x]:
			move_piece (selected, [y, x])
			selected = None
			possible_moves = set ()
	if selected != [y, x]:
		selected = None
		possible_moves = set ()
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

	draw_board ()


	pygame.display.flip ()

pygame.quit ()