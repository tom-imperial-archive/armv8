#include <stdbool.h>
#include "common/util.h"
#include "emulator/state/state.h"
#include "emulator/decode.h"
#include <stdio.h>
#define BIT_63_MASK 1UL << 63
#define BIT_31_MASK 1UL << 31



void pstate_n_64(State *state, uint64 res) {
    write_pstate_flag(state, N, res >> 63);
}

void pstate_n_32(State *state, uint32 res) {
    write_pstate_flag(state, N, res >> 31);
}

void pstate_z_eq_zero(State *state, uint64 res) {
    write_pstate_flag(state, Z, res == 0);
}

void pstate_c(State *state, uint64 res, uint64 original) {
    write_pstate_flag(state, C, res < original);
}

void pstate_v_64(State *state, uint64 res, uint64 original, uint64 imm) {
    write_pstate_flag(state, V, (original ^ res) >> 63 && (imm ^ res) >> 63);
}

void pstate_v_32(State *state, uint32 res, uint32 original, uint32 imm) {
    write_pstate_flag(state, V, (original ^ res) >> 31 && (imm ^ res) >> 31);
}

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
        uint64 valXn = read_reg_64(state, iai.rn);
        uint64 res = imm + valXn;
        write_reg_64(state, iai.rd, res);
        pstate_n_64(state, res);
        pstate_z_eq_zero(state, res);
        pstate_c(state, res, valXn);
        pstate_v_64(state, res, valXn, imm);
    }
    else
    {
        uint32 valWn = read_reg_32(state, iai.rn);
        uint32 res = imm + valWn;
        write_reg_32(state, iai.rd, res);
        printf("Setting n with %x\n", res);
        pstate_n_32(state, res);
        pstate_z_eq_zero(state, res);
        pstate_c(state, res, valWn);
        pstate_v_32(state, res, valWn, imm);
    }
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
    case OP_TYPE_SUB:
        break;
    case OP_TYPE_SUBS:
        break;
    default: break;
        // Data processing instruction (register)
    }
    return false;
}
