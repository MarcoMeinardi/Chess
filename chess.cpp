#include "./game.h"

int main () {
    Game* game = new Game ();
    char command;
    int pos, from, to;
    int moves[128];
    int n_moves;

    while (true) {
        cin >> command;

        switch (command) {
        case 'S':   // select piece
            cin >> pos;
            n_moves = game->get_piece_moves_GUI (pos, moves);
            cout << n_moves << endl;
            for (int i = 0; i < n_moves; i++) {
                cout << (moves[i] >> 8) << endl; // don't need "from"
            }
            break;
        case 'M':   // move piece
            cin >> from >> to;
            cout << game->move_piece_GUI (from, to) << endl;
            break;
        }
    }

    return 0;
}