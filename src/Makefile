CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror -pedantic -I.

.SUFFIXES: .c .o

.PHONY: all clean test

all: assemble emulate

assemble: assemble.o
emulate: emulate.o hashset.o

test:
	$(CC) $(CFLAGS) ../tests/test_hashset.c common/hashset.c -o test_runner
	./test_runner

clean:
	$(RM) *.o assemble emulate

