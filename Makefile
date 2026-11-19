CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -g

.PHONY: all clean run

all: dict

dict: main.o dictionary.o
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c dictionary.h
	$(CC) $(CFLAGS) -c main.c

dictionary.o: dictionary.c dictionary.h
	$(CC) $(CFLAGS) -c dictionary.c

run: dict
	./dict

clean:
	rm -f dict dict.exe main.o dictionary.o
