#ifndef EXECUTE_H
#define EXECUTE_H

#include "emulator/decode/decode.h"
#include "emulator/state/state.h"
#include <stdbool.h>

/*
    Executes the given instruction.
    If we encounter a halt instruction, we return true, otherwise return false;
*/
bool execute_instruction(State *state, Instruction *i);

void execute_single_data_transfer(State *state, Instruction *i);
void execute_load_literal(State *state, Instruction *i);

#endif
