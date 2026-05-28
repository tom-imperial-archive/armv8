#include "utils/types.h"
#include "common/instruction.h"

/*
Here, we take an instruction, represented by a struct, and turn it into the actual 32-bit representation.
This will ultimately be achieved by bitwise operations and masks.

Essentially, we will have one big switch, like we did in execute,
    but the work being done is like the opposite of what was done in decode.
*/

// Takes a fully populated instruction struct and packs it into a 32 bit binary instruction
// This is the exact opposite of what we did in decode
uint32 encode_instruction(Instruction *i) {
    return 0;
}
