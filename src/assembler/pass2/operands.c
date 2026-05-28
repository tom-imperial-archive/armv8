#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils/types.h"
#include "common/instruction.h"

/*
The functions will be of great use when parsing the assembly into structs.
We will require (at least): register parsing, immediate value parsing, memory offset calculatimg.
*/

// Parses a register string e.g. "x0" or "w12"
// Returns the register number, and sets the boolean pointer depending on the register size
int parse_register(char *token, bool *is_64_bit) {
    if (strcmp(token, "sp") == 0) {
        *is_64_bit = true;
        return 31;
    }

    char size = token[0];
    switch (size) {
        case 'x':
            *is_64_bit = true;
            break;
        case 'w':
            *is_64_bit = false;
            break;
        default:
            printf("Error: Invalid register prefix '%c' in '%s'\n", size, token);
            exit(EXIT_FAILURE);
    }

    if (strcmp(token + 1, "zr") == 0) {
        return 31;
    }

    return atoi(token+1);
}

// Parses an immediate value string in decimal or hex e.g. #5, #0x1A, or #-12
long parse_immediate(char *token) {
    if (token[0] != '#') {
        fprintf(stderr, "Error: Invalid immediate format '%s' (missing '#')\n", token);
        exit(EXIT_FAILURE);
    }

    return strtol(token + 1, NULL, 0);
}

// Calculates the branch offset for branch instructions
// Returns the offset in terms of number of instructions
int calculate_offset(uint64 current_address, uint64 target_address) {
    return (int)((target_address-current_address) / 4);
}

// Parses a shift type string and returns the enum
ShiftType parse_shift(char *token) {
    if (strcmp(token, "lsl") == 0) return SHIFT_LSL;
    if (strcmp(token, "lsr") == 0) return SHIFT_LSR;
    if (strcmp(token, "asr") == 0) return SHIFT_ASR;
    if (strcmp(token, "ror") == 0) return SHIFT_ROR;

    printf("Error: Unknown shift type '%s'\n", token);
    exit(EXIT_FAILURE);
}
