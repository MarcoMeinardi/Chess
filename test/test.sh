#!/bin/sh

echo Compiling...
g++ -Wall -o test test.cpp ../game.cpp -O3 -finline-functions -flto -march=native

echo 
echo Start test
echo 
time ./test
echo 
echo Done

rm test