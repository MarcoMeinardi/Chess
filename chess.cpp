#include "./game.h"

int main () {
    Game game;

    game.move_piece ((1 << 4) | (5 & 0xf), (3 << 4) | (5 & 0xf));

    game.move_piece ((0 << 4) | (1 & 0xf), (2 << 4) | (2 & 0xf));
    game.print_board ();
    return 0;
}