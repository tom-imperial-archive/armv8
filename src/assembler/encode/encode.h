#ifndef ENCODE_H
#define ENCODE_H

#include "common/instruction.h"
#include "utils/types.h"

// Takes a fully populated instruction struct and packs it into a 32 bit binary
// instruction This is the exact opposite of what we did in decode
uint32 encode_instruction(Instruction *i);

#endif
