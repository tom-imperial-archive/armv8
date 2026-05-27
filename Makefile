CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror -pedantic -Isrc

$(shell mkdir -p bin)

# SOURCE FILES
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

TEST_UTILS_SRCS = \
	tests/utils/test_hashset.c \
	src/utils/hashset.c

TEST_STATE_SRCS = \
	tests/emulator/state/test_state.c \
	src/emulator/state/memory.c \
	src/emulator/state/state.c \
	src/utils/hashset.c

TEST_DECODE_SRCS = \
	tests/emulator/decode/test_decode.c \
	src/emulator/decode/decode.c

TEST_EXECUTE_SRCS = \
	tests/emulator/execute/test_execute.c \
	tests/emulator/execute/test_dpii.c \
	tests/emulator/execute/test_dpir.c \
	tests/emulator/execute/test_branch.c \
	tests/emulator/execute/test_load_literal.c \
	tests/emulator/execute/test_single_data_transfer.c \
	src/emulator/execute/execute.c \
	src/emulator/state/state.c \
	src/emulator/state/memory.c \
	src/utils/hashset.c


# OBJECT FILES (source files with extensions changed)
EMULATE_OBJS = $(EMULATE_SRCS:.c=.o)
ASSEMBLE_OBJS = $(ASSEMBLE_SRCS:.c=.o)
TEST_UTILS_OBJS = $(TEST_UTILS_SRCS:.c=.o)
TEST_STATE_OBJS = $(TEST_STATE_SRCS:.c=.o)
TEST_DECODE_OBJS = $(TEST_DECODE_SRCS:.c=.o)
TEST_EXECUTE_OBJS = $(TEST_EXECUTE_SRCS:.c=.o)


# BUILD TARGETS AND RULES
.PHONY: all clean test test_execute

all: bin/emulate bin/assemble

bin/emulate: $(EMULATE_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

bin/assemble: $(ASSEMBLE_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Compiles any missing .o file, and places next to its .c counterpart
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# TEST LINKING
bin/test_utils: $(TEST_UTILS_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

bin/test_state: $(TEST_STATE_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

bin/test_decode: $(TEST_DECODE_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

bin/test_execute: $(TEST_EXECUTE_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# TEST TARGETS
test-utils: bin/test_utils
	./bin/test_utils

test-state: bin/test_state
	./bin/test_state

test-decode: bin/test_decode
	./bin/test_decode

test-execute: bin/test_execute
	./bin/test_execute

# COMBINED TESTS
test-emulator: test-state test-decode test-execute
test-all: test-utils test-emulator

# CLEAN
clean:
	-$(RM) -r bin/*
	-$(RM) $(EMULATE_OBJS) $(ASSEMBLE_OBJS)
	-$(RM) $(TEST_UTILS_OBJS) $(TEST_STATE_OBJS) $(TEST_DECODE_OBJS) $(TEST_EXECUTE_OBJS)

