#ifndef ERROR_H_
#define ERROR_H_
#include <stdio.h>
#include "utils/types.h"

#define ERROR(...) _error(__VA_ARGS__, __FILE__, __LINE__, __func__)

static const int max_file_name_length = 32;

typedef enum
{
    FAILED_TO_ALLOCATE,
    ERROR_READING_FILE,
    ERROR_WRITING_FILE,
    FILE_SIZE_NOT_MULTIPLE_OF_4,
    STATE_REG_NOT_EXISTS,
    STATE_WRITE_NOT_ALLOWED,
    STATE_32_BIT_READ_FROM_PC,
    STATE_FLAG_NOT_EXISTS,
    UNDEFINED_OPCODE,
    REQUIRE_CORRECT_ARGS,
    ADDRESSING_MODE_NOT_RECOGNISED,
    NOT_ALL_WORDS_READ,
    INCORRECT_OP_TYPE,
    UNRECOGNISED_INSTRUCTION_TYPE,
    DPI_INVALID_SHIFT,
    INVALID_SHIFT_TYPE,
    REGISTER_SIZE_MISMATCH,
    ILLEGAL_STR_ADDRESSING,
    OFFSET_MULTIPLE_N,
    ILLEGAL_WIDE_MOVE_SIZE,
    INVALID_SHIFT_AMOUNT,
    WIDE_MOVE_REQUIRES_LSL,
    INT_DIRECTIVE_REQUIRES_VALUE,
    UNKNOWN_MNENOMIC,
    INVALID_REGISTER_PREFIX,
    INVALID_IMMEDIATE_FORMAT_HASH
} ErrorType;

typedef struct {
    ErrorType type;
    union {
        int index;
        // Must have length <= max_file_name_length (including the string terminator \0)
        char *str;
        uint32 instruction;
        long shift_amount;
    };
} Error;

void _error(Error error, const char *file, int line, const char *func);

#endif
