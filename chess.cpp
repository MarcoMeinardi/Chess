#include "./game.h"

int main () {
    Game game;

    while (true) {
        game.print_board ();
        game.print_possible_moves ();

        cout << "> ";
        string from, to;
        cin >> from >> to;
        game.human_move (from + " " + to);
    }
    return 0;
}