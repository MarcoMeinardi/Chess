#include "./game.h"

int main () {
    Game game;

    game.move_piece (1 | (5 << 3), 3 | (5 << 3));

    game.move_piece (0 | (1 << 3), 2 | (3 << 3));
    game.print_board ();
    return 0;
}