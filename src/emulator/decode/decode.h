#ifndef DECODE_H
#define DECODE_H

#include "common/instruction.h"
#include "utils/types.h"
#include <stdbool.h>

typedef enum DecodeResult {
    DECODE_SUCCESS,
    DECODE_UNDEFINED_OPCODE, // OPI, OPC, OPR are not supported in the emulator
} DecodeResult;

extern DecodeResult decode(uint32 input, Instruction *result);

#endif
