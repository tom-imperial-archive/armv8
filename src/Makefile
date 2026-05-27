CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror -pedantic -I.

.SUFFIXES: .c .o

.PHONY: all clean test test_execute

all: assemble emulate

$(objects): %.o: %.c
	$(CC) -c $^ -o $@

assemble: assemble.o
emulate: emulate.o common/hashset.o emulator/state/state.o emulator/state/memory.o emulator/decode/filehandlers.o emulator/decode.o emulator/decode/execute.o emulator/output.o

test:
	$(CC) $(CFLAGS) ../tests/tests.c ../tests/test_hashset.c ../tests/test_load_literal.c ../tests/test_single_data_transfer.c emulator/decode/execute.c  ../tests/test_dpir.c ../tests/test_dpii.c ../tests/test_branch.c ../tests/test_state.c emulator/state/state.c emulator/state/memory.c common/hashset.c emulator/decode.c ../tests/test_decode.c -o test_runner
	./test_runner
test_execute:
	$(CC) $(CFLAGS) ../tests/test_execute.c ../tests/test_dpii.c ../tests/test_dpir.c ../tests/test_branch.c emulator/state/state.c emulator/decode/execute.c emulator/state/memory.c common/hashset.c emulator/decode.c ../tests/test_load_literal.c ../tests/test_single_data_transfer.c -o test_execute_runner
	./test_execute_runner
clean:
	$(RM) *.o assemble emulate test_runner test_execute_runner

