#ifndef DECODE_H
#define DECODE_H

#include "common/instruction.h"
#include "utils/types.h"
#include <stdbool.h>

extern void decode(uint32 input, Instruction *instruction);

#endif
