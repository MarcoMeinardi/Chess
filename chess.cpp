#include "./game.h"

int main () {
    Game* game;

    // standard game (good luck)
    // while (true) {
    //     game->print_board ();
    //     game->print_possible_moves ();

    //     cout << "> ";
    //     string from, to;
    //     cin >> from >> to;
    //     game->human_move (from + " " + to);
    // }
    // free (game);

    // test
    srand (time (NULL));
    int total_moves = 0;
    for (int i = 0; i < 1000; i++) {
        game = new Game ();
        int res = game->test ();
        free (game);
        int cnt = res & ~(7 << 29);
        bool last_turn = res & (1 << 29);
        bool is_checkmate = res & (1 << 30);
        bool is_draw = res & (1 << 31);

        cout << i << ":" << endl;
        cout << "-----------" << endl;
        cout << cnt << " moves" << endl;
        cout << last_turn << " made the last move" << endl;
        if (is_checkmate) {
            cout << "checkmate" << endl;
        } else if (is_draw) {
            cout << "draw" << endl;
        } else {
            cout << "stopped" << endl;
        }
        cout << endl;
        total_moves += cnt;
    }

    cout << "-----------" << endl;
    cout << total_moves << " moves made in total" << endl;
    return 0;
}