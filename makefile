chess:
	g++ -Wall -o chess chess.cpp game.cpp -O3 -finline-functions -flto -march=native