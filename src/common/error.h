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
    NOT_ALL_WORDS_READ
} ErrorType;

union ErrorReason
{
    int index;
    // Must have length <= max_file_name_length (including the string terminator \0)
    char *file_name;
    uint32 instruction;
};

typedef union ErrorReason* ErrorInfo;

void error(ErrorType error, ErrorInfo info);
void print_err(ErrorType error, ErrorInfo info);

ErrorInfo int_error_info(int index);
ErrorInfo file_error_info(char *path);
ErrorInfo instruction_error_info(uint32 instruction);

#endif
