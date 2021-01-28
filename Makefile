# The make file for the kilo program
CC = g++
CFLAG = -Wall -Wextra -pedantic -std=c++11

kilo: kilo.o main.o kilo.h
	$(CC) main.o kilo.o -o kilo $(CFLAG)

main.o: main.cpp
	$(CC) -c main.cpp -o main.o $(CFLAG)

kilo.o: kilo.cpp
	$(CC) -c kilo.cpp -o kilo.o $(CFLAG)


clean:
	rm -rf *.o kilo
