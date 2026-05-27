CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror -pedantic -Isrc

$(shell mkdir -p bin)

# Source files
EMULATE_SRCS = \
	src/emulate/emulate.c \
	src/utils/hashset.c \
	src/emulator/state/state.c \
	src/emulator/state/memory.c \
	src/emulator/execute/execute.c \
	src/emulator/decode/decode.c \
	src/emulator/io/filehandlers.c \
	src/emulator/io/output.c

ASSEMBLE_SRCS = \
	src/assemble/assemble.c

# Objects are source files with extensions changed
EMULATE_OBJS = $(EMULATE_SRCS:.c=.o)
ASSEMBLE_OBJS = $(assemble:.c=.o)

.PHONY: all clean test test_execute

all: bin/emulate bin/assemble

# $^ refers to all .o files, $@ means the target name
bin/emulate: $(EMULATE_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

bin/assemble: $(ASSEMBLE_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Ensure .o files are placed next to their .c counterparts
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


# Testing logic
test:
	$(CC) $(CFLAGS) \
		tests/tests.c \
		tests/utils/test_hashset.c \
		tests/emulator/execute/test_load_literal.c \
		tests/emulator/execute/test_single_data_transfer.c \
		tests/emulator/execute/test_dpir.c \
		tests/emulator/execute/test_dpii.c \
		tests/emulator/execute/test_branch.c \
		tests/emulator/state/test_state.c \
		tests/emulator/decode/test_decode.c \
		src/emulator/decode/execute.c \
		src/emulator/state/state.c \
		src/emulator/state/memory.c \
		src/utils/hashset.c \
		src/emulator/decode/decode.c \
		-o bin/test_runner
	./bin/test_runner

test_execute:
	$(CC) $(CFLAGS) \
		tests/emulator/execute/test_execute.c \
		tests/emulator/execute/test_dpii.c \
		tests/emulator/execute/test_dpir.c \
		tests/emulator/execute/test_branch.c \
		tests/emulator/execute/test_load_literal.c \
		tests/emulator/execute/test_single_data_transfer.c \
		src/emulator/state/state.c \
		src/emulator/decode/execute.c \
		src/emulator/state/memory.c \
		src/utils/hashset.c \
		src/emulator/decode/decode.c \
		-o bin/test_execute_runner
	./bin/test_execute_runner

# Clean
clean:
	-$(RM) -r bin/*
	-$(RM) $(EMULATE_OBJS) $(ASSEMBLE_OBJS)

