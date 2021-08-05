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
    int total_games = 100'000;
    int total_moves = 0;
    int black_win = 0, white_win = 0;
    int checkmates = 0, draws = 0;
    
    for (int i = 0; i < total_games; i++) {
        if (i % 1'000 == 0) {
            cout << "Games played:\t" << i << endl;
            cout << "Total moves:\t" << total_moves << endl;
            cout << "Checkmates:\t" << checkmates << " / " << i << endl;
            cout << "Draws:\t\t" << draws << " / " << i << endl;
            cout << "White wins:\t" << white_win << " / " << checkmates << endl;
            cout << "Black wins:\t" << black_win << " / " << checkmates << endl;
            cout << endl;
        }


        game = new Game ();
        int res = game->test ();
        free (game);
        int cnt = res & ~(7 << 29);
        bool last_turn = res & (1 << 29);
        bool is_checkmate = res & (1 << 30);
        bool is_draw = res & (1 << 31);

        if (cnt >= 9999) {
            cout << "stopped" << endl;
            cout << cnt << endl;
            return 0;
        }

        if (is_checkmate) {
            checkmates++;
            if (last_turn == WHITE) {
                white_win++;
            } else {
                black_win++;
            }
        } else if (is_draw) {
            draws++;
        } else {
            cout << "Somthing strange has happened (maybe stopped???)" << endl;
        }
        total_moves += cnt;
    }

    cout << endl << "-------------------" << endl;
    cout << "Final stats" << endl << endl;
    cout << "Games played:\t" << total_games << endl;
    cout << "Total moves:\t" << total_moves << endl;
    cout << "Checkmates:\t" << checkmates << " / " << total_games << endl;
    cout << "Draws:\t\t" << draws << " / " << total_games << endl;
    cout << "White wins:\t" << white_win << " / " << checkmates << endl;
    cout << "Black wins:\t" << black_win << " / " << checkmates << endl;
    return 0;
}