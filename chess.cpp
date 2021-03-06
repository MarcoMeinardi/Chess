#include "./game.h"
#include <ctime>
#include <chrono>

int main () {
	srand (time (NULL));

	Game* game = new Game ();
	char command;
	int pos, from, to;
	int moves[128];
	int n_moves;
	int best_move;

	while (true) {
		cin >> command;

		switch (command) {
		case 'S':   // select piece
			cin >> pos;
			n_moves = game->get_piece_moves_GUI (pos, moves);
			cout << n_moves << endl;
			for (int i = 0; i < n_moves; i++) {
				cout << ((moves[i] >> 8) & 0xff) << endl; // don't need "from" and promotion
			}
			break;
		case 'M':   // move piece
			cin >> from >> to;
			cout << game->move_piece_GUI (from, to) << endl;
			break;
		case 'A':	// automatic move
			int best_score;
			int depth = 5;
			while (true) {
				auto t1 = chrono::high_resolution_clock::now ();
				best_score = game->get_best_move (&best_move, depth);
				auto t2 = chrono::high_resolution_clock::now ();
				chrono::duration <double, milli> time = t2 - t1;
				if (time.count () > 500 ||						// taken > 0.5 seconds
					best_score >= INF || best_score <= -INF ||	// checkmate
					(best_score == 0 && depth >= 10)			// probably draw
				) {
					break;
				}
				depth++;
			}
			cerr << game->get_best_move (&best_move, 5) << endl;
			DECOMPRESS_MOVE (best_move, from, to);
			cout << from << endl << to << endl;
			cout << game->move_piece_GUI (from, to) << endl;
			break;
		}
	}

	delete game;

	return 0;
}