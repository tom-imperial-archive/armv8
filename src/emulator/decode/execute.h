#include <stdbool.h>
#include "emulator/state/state.h"
#include "emulator/decode.h"

/*
    Executes the given instruction.
    If we encounter a halt instruction, we return true, otherwise return false;
*/
bool execute_instruction(State *state, OpType op, Instruction i);
