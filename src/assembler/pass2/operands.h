#ifndef OPERANDS_H
#define OPERANDS_H

#include "common/instruction.h"
#include "utils/types.h"
#include <stdbool.h>

// Parses a register string e.g. "x0" or "w12"
// Returns the register number, and sets the boolean pointer depending on the
// register size
extern int parse_register(char *token, bool *is_64_bit);

// Parses an immediate value string in decimal or hex e.g. #5, #0x1A, or #-12
extern long parse_immediate(char *token);

// Calculates the branch offset for branch instructions
// Returns the offset in terms of number of instructions
extern int calculate_offset(uint64 current_address, uint64 target_address);

// Parses a shift type string and returns the enum
extern ShiftType parse_shift(char *token);

#endif
