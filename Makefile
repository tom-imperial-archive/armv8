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
	src/emulator/io/output.c \
	src/common/instruction.c \
	src/common/error.c

ASSEMBLE_SRCS = \
	src/assemble/assemble.c \
	src/assembler/encode/encode.c \
	src/assembler/pass1/scanner.c \
	src/assembler/pass2/parser.c \
	src/assembler/pass2/operands.c \
	src/assembler/symbol_table/symbol_table.c \
	src/common/instruction.c \
	src/common/error.c

TEST_UTILS_SRCS = \
	tests/utils/test_hashset.c \
	src/utils/hashset.c \
	src/common/error.c

TEST_STATE_SRCS = \
	tests/emulator/state/test_state.c \
	src/emulator/state/memory.c \
	src/emulator/state/state.c \
	src/utils/hashset.c \
	src/common/error.c

TEST_DECODE_SRCS = \
	tests/emulator/decode/test_decode.c \
	src/emulator/decode/decode.c \
	src/common/error.c

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
	src/utils/hashset.c \
	src/common/instruction.c \
	src/common/error.c

TEST_ENCODE_SRCS = \
	tests/assembler/encode/test_encode.c \
	src/assembler/encode/encode.c \
	src/common/instruction.c \
	src/common/error.c

TEST_PASS1_SRCS = \
	tests/assembler/pass1/test_scanner.c \
	src/assembler/pass1/scanner.c \
	src/assembler/symbol_table/symbol_table.c \
	src/common/error.c

TEST_PARSER_SRCS = \
    tests/assembler/pass2/test_parser.c \
    src/assembler/pass2/parser.c \
    src/assembler/pass2/operands.c \
	src/assembler/symbol_table/symbol_table.c \
	src/common/error.c

TEST_OPERANDS_SRCS = \
    tests/assembler/pass2/test_operands.c \
    src/assembler/pass2/operands.c \
	src/common/error.c

TEST_SYMBOL_TABLE_SRCS = \
	tests/assembler/symbol_table/test_symbol_table.c \
	src/assembler/symbol_table/symbol_table.c \
	src/common/error.c

# OBJECT FILES (source files with extensions changed)
EMULATE_OBJS = $(EMULATE_SRCS:.c=.o)
ASSEMBLE_OBJS = $(ASSEMBLE_SRCS:.c=.o)
TEST_UTILS_OBJS = $(TEST_UTILS_SRCS:.c=.o)
TEST_STATE_OBJS = $(TEST_STATE_SRCS:.c=.o)
TEST_DECODE_OBJS = $(TEST_DECODE_SRCS:.c=.o)
TEST_EXECUTE_OBJS = $(TEST_EXECUTE_SRCS:.c=.o)
TEST_ENCODE_OBJS = $(TEST_ENCODE_SRCS:.c=.o)
TEST_PASS1_OBJS = $(TEST_PASS1_SRCS:.c=.o)
TEST_PARSER_OBJS = $(TEST_PARSER_SRCS:.c=.o)
TEST_OPERANDS_OBJS = $(TEST_OPERANDS_SRCS:.c=.o)
TEST_SYMBOL_TABLE_OBJS = $(TEST_SYMBOL_TABLE_SRCS:.c=.o)



# BUILD TARGETS AND RULES
.PHONY: all clean test format

all: bin/emulate bin/assemble

bin/emulate: $(EMULATE_OBJS)
	$(CC) $(CFLAGS) $^ -o $@
emulate: bin/emulate

bin/assemble: $(ASSEMBLE_OBJS)
	$(CC) $(CFLAGS) $^ -o $@
assemble: bin/assemble

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

bin/test_encode: $(TEST_ENCODE_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

bin/test_pass1: $(TEST_PASS1_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

bin/test_parser: $(TEST_PARSER_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

bin/test_operands: $(TEST_OPERANDS_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

bin/test_symbol_table: $(TEST_SYMBOL_TABLE_OBJS)
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

test-encode: bin/test_encode
	./bin/test_encode

test-pass1: bin/test_pass1
	./bin/test_pass1

test-parser: bin/test_parser
	./bin/test_parser

test-operands: bin/test_operands
	./bin/test_operands

test-symbol-table: bin/test_symbol_table
	./bin/test_symbol_table

testsuite:
	./run-testsuite

# COMBINED TESTS
test-emulator: test-state test-decode test-execute

test-pass2: test-parser test-operands
test-assembler: test-encode test-pass1 test-pass2 test-symbol-table

test-all: test-utils test-emulator test-assembler

# FORMAT
format:
	git ls-files '*.c' '*.h' | xargs clang-format -i

# CLEAN
clean:
	-$(RM) -r bin/*
	-$(RM) $(EMULATE_OBJS) $(ASSEMBLE_OBJS)
	-$(RM) $(TEST_UTILS_OBJS) $(TEST_STATE_OBJS) $(TEST_DECODE_OBJS) $(TEST_EXECUTE_OBJS) $(TEST_ENCODE_OBJS) $(TEST_PASS1_OBJS) $(TEST_PARSER_OBJS) $(TEST_OPERANDS_OBJS) $(TEST_SYMBOL_TABLE_OBJS)

