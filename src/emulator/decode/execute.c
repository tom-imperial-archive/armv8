#include <stdbool.h>
#include "common/util.h"
#include "emulator/state/state.h"
#include "emulator/decode.h"


void add_imm(State *state, Instruction *i)
{
    ImmediateArithmeticInstruction iai = i->data.immediate_arithmetic;
    uint64 imm = iai.imm12;
    if (iai.sh)
    {
        imm = imm << 12;
    }

    if (iai.sf)
    {
        uint64 valRn = read_reg_64(state, iai.rn);
        write_reg_64(state, iai.rd, imm + valRn);
    }
    else
    {
        uint32 valRn = read_reg_32(state, iai.rn);
        write_reg_32(state, iai.rd, imm + valRn);
    }
}

void adds_imm(State *state, Instruction *i)
{
    ImmediateArithmeticInstruction iai = i->data.immediate_arithmetic;
    uint32 imm = iai.imm12;
    if (iai.sh)
    {
        imm = imm << 12;
    }

    if (iai.sf)
    {
        uint64 valRn = read_reg_64(state, iai.rn);
        write_reg_64(state, iai.rd, imm + valRn);
    }
    else
    {
        uint32 valRn = read_reg_32(state, iai.rn);
        write_reg_32(state, iai.rd, imm + valRn);
    }

    // todo set flags
}


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
    case OP_TYPE_ADD:
        add_imm(state, i);
        break;
    case OP_TYPE_ADDS:
        adds_imm(state, i);
        break;
    default: break;
        // Data processing instruction (register)
    }
    return false;
}
