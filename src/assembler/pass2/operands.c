#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils/types.h"

/*
The functions will be of great use when parsing the assembly into structs.
We will require (at least): register parsing, immediate value parsing, memory offset calculatimg.
*/

// Parses a register string e.g. "x0" or "w12"
// Returns the register number, and sets the boolean pointer depending on the register size
int parse_register(char *token, bool *is_64_bit) {
    char size = token[0];

    switch (size) {
        case 'x':
            *is_64_bit = true;
            break;
        case 'w':
            *is_64_bit = false;
            break;
        default:
            perror("Invalid register format");
            exit(EXIT_FAILURE);
    }

    if (strcmp(token+1, "zr") == 0) {
        return 31;
    }

    return atoi(token+1);
}

// Parses an immediate value string in decimal or hex e.g. #5, #0x1A, or #-12
long parse_immediate(char *token);

// Calculates the branch offset for branch instructions
// Returns the offset in terms of number of instructions
int calculate_offset(uint64 current_address, uint64 target_address);
