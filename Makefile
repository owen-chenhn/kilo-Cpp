# The make file for the kilo program

# gcc for C program
CC = g++

kilo: kilo.cpp
	$(CC) kilo.cpp -o kilo -Wall -Wextra -pedantic -std=c++11