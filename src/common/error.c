#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#define malloc_error_info() malloc(sizeof(ErrorInfo))

/*
Prints the corresponding output message to the output stream.
Frees info once done, you cannot use the same ErrorInfo twice.
*/

// REVIEW: unsure whether to add a separate function for NULL info
void print_err(ErrorType error, ErrorInfo info)
{
    if (info != NULL)
    {
        switch (error)
        {
        case STATE_REG_NOT_EXISTS:
            fprintf(stderr, "Register indexed %d does not exist\n", info->index);
            break;
        case STATE_WRITE_NOT_ALLOWED:
            fprintf(stderr, "Illegal write to register %d\n", info->index);
            break;
        case STATE_FLAG_NOT_EXISTS:
            fprintf(stderr, "PSTATE flag index %d does not exist\n", info->index);
            break;
        case ERROR_READING_FILE:
            fprintf(stderr, "Failed to read file %s\n", info->file_name);
            break;
        case ERROR_WRITING_FILE:
            fprintf(stderr, "Failed to write to file %s\n", info->file_name);
            break;
        case FILE_SIZE_NOT_MULTIPLE_OF_4:
            fprintf(stderr, "File size not multiple of 4 for %s\n", info->file_name);
            break;
        case NOT_ALL_WORDS_READ:
            fprintf(stderr, "Could not read all words in %s\n", info->file_name);
            break;
        case UNDEFINED_OPCODE:
            fprintf(stderr, "Failed to decode instruction: 0x%016x\n", info->instruction);
            break;
        default: break;
        }
    }
    else
    {
        switch (error)
        {
        case STATE_32_BIT_READ_FROM_PC:
            fprintf(stderr, "Illegal 32-bit read from PC\n");
            break;
        case FAILED_TO_ALLOCATE:
            fprintf(stderr, "Failed to allocate required memory\n");
            break;
        case REQUIRE_CORRECT_ARGS:
            fprintf(stderr, "Require file in: <file_in> [file_out]\n");
            break;
        case ADDRESSING_MODE_NOT_RECOGNISED:
            fprintf(stderr, "Addressing mode not recognized.\n\n");
            break;
        default: fprintf(stderr, "Unknown error occured");
        }
    }

    free(info);
}

/*
Prints the corresponding output message to the output stream, then exits with a failure code
*/
void error(ErrorType error, ErrorInfo info)
{
    print_err(error, info);
    exit(EXIT_FAILURE);
}

ErrorInfo int_error_info(int index)
{
    ErrorInfo info = malloc_error_info();

    if (info != NULL)
    {
        info->index = index;
    }
    return info;
}

ErrorInfo file_error_info(char *path) {
    ErrorInfo info = malloc_error_info();

    if (info != NULL)
    {
        char out[max_file_name_length];
        snprintf(out, max_file_name_length, "%s", path);
        info->file_name = out;
    }
    return info;
}

ErrorInfo instruction_error_info(uint32 instruction) {
    ErrorInfo info = malloc_error_info();

    if (info != NULL)
    {
        info->instruction = instruction;
    }
    return info;
}
