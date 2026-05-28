#include <stdbool.h>
#include "utils/types.h"

/*
The functions will be of great use when parsing the assembly into structs.
We will require (at least): register parsing, immediate value parsing, memory offset calculatimg.
*/

// Parses a register string e.g. "x0" or "w12"
// Returns the register number, and sets the boolean pointer depending on the register size
int parse_register(char *token, bool *is_64_bit);

// Parses an immediate value string in decimal or hex e.g. #5, #0x1A, or #-12
long parse_immediate(char *token);

// Calculates the branch offset for branch instructions
// Returns the offset in terms of number of instructions
int calculate_offset(uint64 current_address, uint64 target_address);
