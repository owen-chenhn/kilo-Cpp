# The make file for the kilo program
CC = g++	# gcc for C program

kilo: kilo.cpp
	$(CC) kilo.cpp -o kilo -Wall -Wextra -pedantic -std=c++11