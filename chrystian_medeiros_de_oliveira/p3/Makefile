CC = gcc
CFLAGS = -Wall -Wextra -std=c99

ECHO = echo "DÉBITO=5+8*8"

all: compilador executor

compilador: compilador.c
	$(CC) $(CFLAGS) -o compilador compilador.c

executor: executor.c
	$(CC) $(CFLAGS) -o executor executor.c

bf: all
	$(ECHO) | ./compilador

run: all
	$(ECHO) | ./compilador | ./executor

clean:
	rm -f compilador executor
