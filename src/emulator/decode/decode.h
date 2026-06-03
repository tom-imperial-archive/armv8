#ifndef PARSE_H_
#define PARSE_H_

#include "common/instruction.h"
#include "utils/types.h"
#include <stdbool.h>

typedef enum DecodeResult {
    DECODE_SUCCESS,
    DECODE_UNDEFINED_OPCODE, // OPI, OPC, OPR are not supported in the emulator
} DecodeResult;

DecodeResult decode(uint32 input, Instruction *result);

#endif
