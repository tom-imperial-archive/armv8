#ifndef ERROR_H_
#define ERROR_H_
#include <stdio.h>
#include "utils/types.h"

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

union ErrorReason
{
    int index;
    // Must have length <= max_file_name_length (including the string terminator \0)
    char *str;
    uint32 instruction;
    long shift_amount;
};

typedef union ErrorReason* ErrorInfo;

void error(ErrorType error, ErrorInfo info);
void print_err(ErrorType error, ErrorInfo info);

ErrorInfo int_error_info(int index);
ErrorInfo str_error_info(char *path);
ErrorInfo instruction_error_info(uint32 instruction);
ErrorInfo shift_error_info(uint64 shift_amount);

#endif
