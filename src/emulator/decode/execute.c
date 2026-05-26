#include <stdbool.h>
#include "common/util.h"
#include "emulator/state/state.h"
#include "emulator/decode.h"
#include <stdio.h>

uint64 shift_value(uint64 val, ShiftType type, int amount, bool sf) {
    if (amount == 0) return val;

    switch (type) {
        case SHIFT_LSL:
            val = val << amount;
            break;
        case SHIFT_LSR:
            val = val >> amount;
            break;
        case SHIFT_ASR:
            // Cast to signed for an arithmetic shift
            if (sf) val = (uint64)(((int64) val) >> amount);
            else val = (uint64)(((int32)val) >> amount);
            break;
        case SHIFT_ROR:
            // Use bitwise OR and normal shifts to simulate a rotate
            if (sf) val = (val >> amount) | (val << (64 - amount));
            else val = (val >> amount) | ((val & 0xFFFFFFFF) << (32 - amount));
            break;
    }
    // Zero-extension neccessary here, since shifts can overwrite top half.
    return sf ? val : (uint32)val;
}

void execute_arithmetic_register(State *state, Instruction *i) {
    // Unpack
    bool sf = i->data.register_arithmetic_logic.sf;
    Register rd = (Register)i->data.register_arithmetic_logic.rd;
    Register rn = (Register)i->data.register_arithmetic_logic.rn;
    Register rm = (Register)i->data.register_arithmetic_logic.rm;

    int shift_amount = i->data.register_arithmetic_logic.operand;
    ShiftType shift_type = i->data.register_arithmetic_logic.shift;
    OpType op = i->op_type;

    // Initial values
    uint64 val_n = sf ? read_reg_64(state, rn) : (uint64)read_reg_32(state, rn);
    uint64 val_m = sf ? read_reg_64(state, rm) : (uint64)read_reg_32(state, rm);

    // Shifting
    uint64 op2 = shift_value(val_m, shift_type, shift_amount, sf);

    // Maths
    uint64 result = 0;
    switch (op) {
        case OP_TYPE_REG_SUB:
        case OP_TYPE_REG_SUBS:
            result = val_n - op2;
            break;
        case OP_TYPE_REG_ADD:
        case OP_TYPE_REG_ADDS:
            result = val_n + op2;
            break;
        default:
            break;
    }
    result = sf ? result : (uint32)result;

    // Storing
    if (sf) {
        write_reg_64(state, rd, result);
    }
    else {
        write_reg_32(state, rd, result);
    }

    // PSTATE Flags
    if (op == OP_TYPE_REG_ADDS || op == OP_TYPE_REG_SUBS) {
        // Store the sign bit to use as a mask later
        uint64 sign_bit = 1ULL << (sf ? 63 : 31);

        // N flag: true if sign bit is a 1
        bool n_flag = (result & sign_bit) != 0;

        // Z flag: true if the result is exactly 0
        bool z_flag = (result == 0);

        bool c_flag;
        bool v_flag;

        if (op == OP_TYPE_REG_SUBS) {
            // C flag: true if no borrow occured
            c_flag = (val_n >= op2);

            // V flag: true if signs of operands are different,
            // and sign of result is different from val_n
            v_flag = (((val_n ^ op2) & sign_bit) && ((val_n ^ result) & sign_bit)) != 0;
            // XOR checks if bits are different
        } else {
            // C flag: true if unsigned overflow
            c_flag = (result < val_n);
            // V flag: true if signs of operands are same,
            // and sign of result is different from val_n
            v_flag = ((~(val_n ^ op2) & sign_bit) && ((val_n ^ result) & sign_bit)) != 0;
        }

        // Write flags
        write_pstate_flag(state, N, n_flag);
        write_pstate_flag(state, Z, z_flag);
        write_pstate_flag(state, C, c_flag);
        write_pstate_flag(state, V, v_flag);
    }
}

void execute_logical_register(State *state, Instruction *i) {
    // Unpack
    bool sf = i->data.register_arithmetic_logic.sf;
    Register rd = (Register)i->data.register_arithmetic_logic.rd;
    Register rn = (Register)i->data.register_arithmetic_logic.rn;
    Register rm = (Register)i->data.register_arithmetic_logic.rm;

    int shift_amount = i->data.register_arithmetic_logic.operand;
    ShiftType shift_type = i->data.register_arithmetic_logic.shift;
    OpType op = i->op_type;

    // Initial values
    uint64 val_n = sf ? read_reg_64(state, rn) : (uint64)read_reg_32(state, rn);
    uint64 val_m = sf ? read_reg_64(state, rm) : (uint64)read_reg_32(state, rm);

    // Shifting
    uint64 op2 = shift_value(val_m, shift_type, shift_amount, sf);

    // Negating
    if (op == OP_TYPE_BIC || op == OP_TYPE_BICS || op == OP_TYPE_ORN || op == OP_TYPE_EON) {
        op2 = ~op2;
    }
    // This would flip the top 32 bits in 32-bit registers to 1s, so we cast
    op2 = sf ? op2 : (uint32)op2;

    // Logic
    uint64 result = 0;
    switch (op) {
        case OP_TYPE_AND:
        case OP_TYPE_ANDS:
        case OP_TYPE_BIC:
        case OP_TYPE_BICS:
            result = val_n & op2;
            break;
        case OP_TYPE_ORR:
        case OP_TYPE_ORN:
            result = val_n | op2;
            break;
        case OP_TYPE_EOR:
        case OP_TYPE_EON:
            result = val_n ^ op2;
            break;
        default:
            break;
    }
    result = sf ? result : (uint32)result;

    // Storing
    if (sf) {
        write_reg_64(state, rd, result);
    }
    else {
        write_reg_32(state, rd, result);
    }

    // PSTATE Flags
    if (op == OP_TYPE_ANDS || op == OP_TYPE_BICS) {
        uint64 sign_bit = 1ULL << (sf ? 63 : 31);
        // N flag: true if sign bit is a 1
        bool n_flag = (result & sign_bit) != 0;

        // Z flag: true if the result is exactly 0
        bool z_flag = (result == 0);

        // C and V flags: always 0
        bool c_flag = 0;
        bool v_flag = 0;

        // Write flags
        write_pstate_flag(state, N, n_flag);
        write_pstate_flag(state, Z, z_flag);
        write_pstate_flag(state, C, c_flag);
        write_pstate_flag(state, V, v_flag);
    }
}

void execute_multiply_register(State *state, Instruction *i) {
    // Unpack
    bool sf = i-> data.multiply.sf;
    Register rm = (Register)i->data.multiply.rm;
    Register rn = (Register)i->data.multiply.rn;
    Register rd = (Register)i->data.multiply.rd;
    Register ra = (Register)i->data.multiply.ra;
    OpType op = i->op_type;

    // Initial values
    uint64 val_m = sf ? read_reg_64(state, rm) : (uint64)read_reg_32(state, rm);
    uint64 val_n = sf ? read_reg_64(state, rn) : (uint64)read_reg_32(state, rn);
    uint64 val_a = sf ? read_reg_64(state, ra) : (uint64)read_reg_32(state, ra);

    // Multiplication
    uint64 result = 0;
    switch (op) {
        case OP_TYPE_MADD:
            result = val_a + (val_n * val_m);
            break;
        case OP_TYPE_MSUB:
            result = val_a - (val_n * val_m);
        default:
            break;
    }
    result = sf ? result : (uint32)result;

    // Storing
    if (sf) {
        write_reg_64(state, rd, result);
    }
    else {
        write_reg_32(state, rd, result);
    }
}

void pstate_n_64(State *state, uint64 res)
{
    write_pstate_flag(state, N, res >> 63);
}

void pstate_n_32(State *state, uint32 res)
{
    write_pstate_flag(state, N, res >> 31);
}

void pstate_z_eq_zero(State *state, uint64 res)
{
    write_pstate_flag(state, Z, res == 0);
}

void pstate_c(State *state, uint64 res, uint64 original)
{
    write_pstate_flag(state, C, res < original);
}

void pstate_v_64(State *state, uint64 res, uint64 original, uint64 imm)
{
    write_pstate_flag(state, V, (original ^ res) >> 63 && (imm ^ res) >> 63);
}

void pstate_v_32(State *state, uint32 res, uint32 original, uint32 imm)
{
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
        pstate_n_32(state, res);
        pstate_z_eq_zero(state, res);
        pstate_c(state, res, valWn);
        pstate_v_32(state, res, valWn, imm);
    }
}

void sub_imm(State *state, Instruction *i)
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
        write_reg_64(state, iai.rd, valRn - imm);
    }
    else
    {
        uint32 valRn = read_reg_32(state, iai.rn);
        write_reg_32(state, iai.rd, valRn - imm);
    }
}

void subs_imm(State *state, Instruction *i)
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
        uint64 res = valXn - imm;
        write_reg_64(state, iai.rd, res);
        pstate_n_64(state, res);
        pstate_z_eq_zero(state, res);
        pstate_c(state, res, valXn);
        pstate_v_64(state, res, valXn, imm);
    }
    else
    {
        uint32 valWn = read_reg_32(state, iai.rn);
        uint32 res = valWn - imm;
        write_reg_32(state, iai.rd, res);
        printf("Setting n with %x\n", res);
        pstate_n_32(state, res);
        pstate_z_eq_zero(state, res);
        pstate_c(state, res, valWn);
        pstate_v_32(state, res, valWn, imm);
    }
}

void wide_moven_imm(State *state, Instruction *i)
{
    WideMoveInstruction iai = i->data.wide_move;
    int shift = iai.hw * 16;
    if (iai.sf)
    {
        write_reg_64(state, iai.rd, ~(iai.imm16 << shift));
    }
    else
    {
        write_reg_32(state, iai.rd, ~(iai.imm16 << shift));
    }
}

void wide_movez_imm(State *state, Instruction *i)
{
    WideMoveInstruction iai = i->data.wide_move;
    if (iai.sf)
    {
        write_reg_64(state, iai.rd, iai.imm16);
    }
    else
    {
        write_reg_32(state, iai.rd, iai.imm16);
    }
}

void wide_movek_imm(State *state, Instruction *i)
{
    WideMoveInstruction iai = i->data.wide_move;
    int shift = iai.hw * 16;
    if (iai.sf)
    {
        uint64 existing = read_reg_64(state, iai.rd);
        printf("%lx\n", (existing & ~(0xFFFFUL << shift)));
        write_reg_64(state, iai.rd, ((uint64) iai.imm16 << shift) | (existing & ~(0xFFFFUL << shift)));
    }
    else
    {
        uint32 existing = read_reg_32(state, iai.rd);
        write_reg_32(state, iai.rd, ((uint32) iai.imm16 << shift) | (existing & ~(0xFFFFU << shift)));
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
            sub_imm(state, i);
            break;
        case OP_TYPE_SUBS:
            subs_imm(state, i);
            break;
        case OP_TYPE_MOVN:
            wide_moven_imm(state, i);
            break;
        case OP_TYPE_MOVZ:
            wide_movez_imm(state, i);
            break;
        case OP_TYPE_MOVK:
            wide_movek_imm(state, i);
            break;
        // Data processing instruction (register)
        case OP_TYPE_REG_SUB:
        case OP_TYPE_REG_SUBS:
        case OP_TYPE_REG_ADD:
        case OP_TYPE_REG_ADDS:
            execute_arithmetic_register(state, i);
            break;
        case OP_TYPE_AND:
        case OP_TYPE_BIC:
        case OP_TYPE_ORR:
        case OP_TYPE_ORN:
        case OP_TYPE_EOR:
        case OP_TYPE_EON:
        case OP_TYPE_ANDS:
        case OP_TYPE_BICS:
            execute_logical_register(state, i);
            break;
        case OP_TYPE_MADD:
        case OP_TYPE_MSUB:
            execute_multiply_register(state, i);
            break;

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
                default:
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


        // TEMP until all instructions have been implemented
        default:
            break;
    }
    return false;
}
