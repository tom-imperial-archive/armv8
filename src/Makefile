CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror -pedantic -I.

.SUFFIXES: .c .o

.PHONY: all clean test

all: assemble emulate

$(objects): %.o: %.c
	$(CC) -c $^ -o $@

assemble: assemble.o
emulate: emulate.o common/hashset.o emulator/decode/readfile.o
	$(CC) $(CFLAGS) $^ -o $@

test:
	$(CC) $(CFLAGS) ../tests/test_hashset.c common/hashset.c -o test_runner
	./test_runner

clean:
	$(RM) *.o assemble emulate

