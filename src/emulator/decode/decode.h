#ifndef DECODE_H
#define DECODE_H

#include "utils/types.h"
#include <stdbool.h>

typedef struct Instruction Instruction;

extern void decode(uint32 input, Instruction *instruction);

#endif
