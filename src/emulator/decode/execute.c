#include <stdbool.h>
#include "common/util.h"
#include "emulator/state/state.h"
#include "emulator/decode.h"

/*
    Executes the given instruction.
    If we encounter a halt instruction, we return true, otherwise return false;
*/
bool execute_instruction(State *state, OpType op, Instruction i)
{
    switch (op)
    {
    case OP_TYPE_HALT:
        return true;

    // Data processing instruction (immediate)
    // Data processing instruction (register)

    // Branch instructions
    case OP_TYPE_EQ:
        if (state->Z == 1) {
            int64_t offset = (int64_t)i.data.cond_branch.simm19*4;
            state->PC += offset;
        }
        break;
    case OP_TYPE_NE:
        if (state->Z == 0) {
            int64_t offset = (int64_t)i.data.cond_branch.simm19*4;
            state->PC += offset;
        }
        break;
    case OP_TYPE_GE:
        if (state->N == state->V) {
            int64_t offset = (int64_t)i.data.cond_branch.simm19*4;
            state->PC += offset;
        }
        break;
    case OP_TYPE_LT:
        if (state->N != state->V) {
            int64_t offset = (int64_t)i.data.cond_branch.simm19*4;
            state->PC += offset;
        }
        break;
    case OP_TYPE_GT:
        if ((state->Z == 0) && (state->N == V)) {
            int64_t offset = (int64_t)i.data.cond_branch.simm19*4;
            state->PC += offset;
        }
        break;
    case OP_TYPE_LE:
        if (!((state->Z == 0) && (state->N == V))) {
            int64_t offset = (int64_t)i.data.cond_branch.simm19*4;
            state->PC += offset;
        }
        break;
    case OP_TYPE_AL:
        int64_t offset = (int64_t)i.data.uncond_branch.simm26*4;
        state->PC += offset;
        break;
    case OP_TYPE_BR:
        state->PC = i.data.reg_branch.xn;
        break;
    }

    return false;
}
