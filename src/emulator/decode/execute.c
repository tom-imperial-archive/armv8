#include <stdbool.h>
#include "common/util.h"
#include "emulator/state/state.h"
#include "emulator/decode.h"

/*
    Executes the given instruction.
    If we encounter a halt instruction, we return true, otherwise return false;
*/
bool execute_instruction(State *state, OpType op, Instruction *i)
{
    switch (op)
    {
    case OP_TYPE_HALT:
        return true;

    // Data processing instruction (immediate)
    // Data processing instruction (register)

    // Branch instructions
    // Conditional branches
    case OP_TYPE_EQ:
    case OP_TYPE_NE:
    case OP_TYPE_GE:
    case OP_TYPE_LT:
    case OP_TYPE_GT:
    case OP_TYPE_LE:
        // Read PSTATE flags
        bool z = read_pstate_flag(state, Z);
        bool n = read_pstate_flag(state, N);
        bool v = read_pstate_flag(state, V);

        // Check condition
        bool condition_met = false;
        switch (op) {
            case OP_TYPE_EQ:
                condition_met = (z == 1);
                break;
            case OP_TYPE_NE:
                condition_met = (z == 0);
                break;
            case OP_TYPE_GE:
                condition_met = (n == v);
                break;
            case OP_TYPE_LT:
                condition_met = (n != v);
                break;
            case OP_TYPE_GT:
                condition_met = ((z == 0) && (n == v));
                break;
            case OP_TYPE_LE:
                condition_met = !((z == 0) && (n == v));
                break;
        }

        if (condition_met) {
            int64 offset = (int64)i->data.cond_branch.simm19 * 4;
            state->PC += offset;
        }
        break;

    // Unconditional branch
    case OP_TYPE_AL: {
        int64 offset = (int64)i->data.uncond_branch.simm26 * 4;
        state->PC += offset;
        break;
    }

    // Register branch
    case OP_TYPE_BR:
        state->PC = read_reg_64(state, i->data.reg_branch.xn);
        break;
    }

    return false;
}
