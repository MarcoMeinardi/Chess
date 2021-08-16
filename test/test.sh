#!/bin/sh

echo Random test
echo 
echo Compiling...
g++ -Wall -o random_test random_test.cpp ../game.cpp -O3 -finline-functions -flto -march=native

echo 
echo Start test
echo 
time ./random_test
echo 
echo Done
rm random_test

echo
echo ------------------------
echo 

echo Auto test
echo 
echo Compiling...
g++ -Wall -o auto_vs_auto auto_vs_auto.cpp ../game.cpp -O3 -finline-functions -flto -march=native

echo 
echo Start test
echo 
time ./auto_vs_auto
echo 
echo Done
rm auto_vs_auto


# random test:

# Games played:	10000
# Total moves:	3165841
# Checkmates:	1396 / 10000
# Draws:		8604 / 10000
# White wins:	699 / 1396
# Black wins:	697 / 1396

# 4.70 user 
# 0.00 system 
# 0:04.70 elapsed


# auto test

# Games played:	100
# Total moves:	15486
# Checkmates:	58 / 100
# Draws:		42 / 100
# White wins:	27 / 58
# Black wins:	31 / 58

# 448.38 user 
# 0.01 system 
# 7:28.56elapsed