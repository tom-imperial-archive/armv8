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
            fprintf(stderr, "Failed to read file %s\n", info->str);
            break;
        case ERROR_WRITING_FILE:
            fprintf(stderr, "Failed to write to file %s\n", info->str);
            break;
        case FILE_SIZE_NOT_MULTIPLE_OF_4:
            fprintf(stderr, "File size not multiple of 4 for %s\n", info->str);
            break;
        case NOT_ALL_WORDS_READ:
            fprintf(stderr, "Could not read all words in %s\n", info->str);
            break;
        case UNDEFINED_OPCODE:
            fprintf(stderr, "Failed to decode instruction: 0x%016x\n", info->instruction);
            break;
        case DPI_INVALID_SHIFT:
            fprintf(stderr, "DPI shift must be 0 or 12: %ld\n", info->shift_amount);
            break;
        case INVALID_SHIFT_TYPE:
            fprintf(stderr, "Unknown shift type: %s\n", info->str);
            break;
        case ILLEGAL_STR_ADDRESSING:
            fprintf(stderr, "Str cannot use literal addressing: %s\n", info->str);
            break;
        case OFFSET_MULTIPLE_N:
            fprintf(stderr, "Offset must be multiple of %d\n", info->index);
            break;
        case ILLEGAL_WIDE_MOVE_SIZE:
            fprintf(stderr, "Wide move immediate must be a 16-bit unsigned value (0 to 65535): 0x%lx", info->shift_amount);
            break;
        case INVALID_SHIFT_AMOUNT:
            fprintf(stderr, "Invalid shift amount for wide move (must be 0, 16, 32, or 48): 0x%lx\n", info->shift_amount);
            break;
        case UNKNOWN_MNENOMIC:
            fprintf(stderr, "Unknown mnemonic: %s\n", info->str);
            break;
        case INVALID_REGISTER_PREFIX:
            fprintf(stderr, "Invalid register prefix: %s\n", info->str);
            break;
        case INVALID_IMMEDIATE_FORMAT_HASH:
            fprintf(stderr, "Error: Invalid immediate format '%s' (missing '#')\n", info->str);
            break;
        default: fprintf(stderr, "Unknown error occured");
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
            fprintf(stderr, "Addressing mode not recognised\n");
            break;
        case INCORRECT_OP_TYPE:
            fprintf(stderr, "Incorrect OP_TYPE\n\n");
            break;
        case UNRECOGNISED_INSTRUCTION_TYPE:
            fprintf(stderr, "Unrecognised instruction type\n");
            break;
        case REGISTER_SIZE_MISMATCH:
            fprintf(stderr, "Register size mismatch between Rd and Rn\n");
            break;
        case WIDE_MOVE_REQUIRES_LSL:
            fprintf(stderr, "Wide move shift type must be LSL\n");
            break;
        case INT_DIRECTIVE_REQUIRES_VALUE:
            fprintf(stderr, ".int directive requires a value\n");
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

ErrorInfo str_error_info(char *str) {
    ErrorInfo info = malloc_error_info();

    if (info != NULL)
    {
        char out[max_file_name_length];
        snprintf(out, max_file_name_length, "%s", str);
        info->str = str;
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

ErrorInfo shift_error_info(uint64 shift_amount) {
    ErrorInfo info = malloc_error_info();

    if (info != NULL)
    {
        info->shift_amount = shift_amount;
    }
    return info;
}
