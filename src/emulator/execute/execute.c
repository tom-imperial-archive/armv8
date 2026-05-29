#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "utils/types.h"
#include "utils/bitmasks.h"
#include "emulator/state/state.h"
#include "emulator/decode/decode.h"
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
            else val = (val >> amount) | ((val & BITMASK_LOWER_32_BITS) << (32 - amount));
            break;
    }
    // Zero-extension neccessary here, since shifts can overwrite top half.
    return sf ? val : (uint32)val;
}

// Shared helper function for register and immediate data processing instructions
void process_arithmetic(State *state, Register rd, uint64 val_n, uint64 op2, bool is_sub, bool set_flags, bool sf) {
    // Maths
    uint64 result = is_sub ? (val_n - op2) : (val_n + op2);
    result = sf ? result : (uint32)result;

    // Storing
    if (sf) {
        write_reg_64(state, rd, result);
    }
    else {
        write_reg_32(state, rd, result);
    }

    // PSTATE Flags
    if (set_flags) {
        // Store the sign bit to use as a mask later
        uint64 sign_bit = 1ULL << (sf ? 63 : 31);

        // N flag: true if sign bit is a 1
        bool n_flag = (result & sign_bit) != 0;

        // Z flag: true if the result is exactly 0
        bool z_flag = (result == 0);

        bool c_flag, v_flag;

        if (is_sub) {
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

void execute_arithmetic_register(State *state, Instruction *i) {
    // Unpack
    bool sf = i->data.register_arithmetic_logic.sf;
    Register rd = (Register)i->data.register_arithmetic_logic.rd;
    Register rn = (Register)i->data.register_arithmetic_logic.rn;
    Register rm = (Register)i->data.register_arithmetic_logic.rm;

    int shift_amount = i->data.register_arithmetic_logic.operand;
    ShiftType shift_type = i->data.register_arithmetic_logic.shift;

    // Initial values
    uint64 val_n = sf ? read_reg_64(state, rn) : (uint64)read_reg_32(state, rn);
    uint64 val_m = sf ? read_reg_64(state, rm) : (uint64)read_reg_32(state, rm);

    // Shifting
    uint64 op2 = shift_value(val_m, shift_type, shift_amount, sf);

    // Determine operation
    OpType op = i->op_type;
    bool is_sub = (op == OP_TYPE_REG_SUB || op == OP_TYPE_REG_SUBS);
    bool set_flags = (op == OP_TYPE_REG_ADDS || op == OP_TYPE_REG_SUBS);

    // Call handler
    process_arithmetic(state, rd, val_n, op2, is_sub, set_flags, sf);
}

void execute_arithmetic_immediate(State *state, Instruction *i) {
    // Unpack
    bool sf = i->data.immediate_arithmetic.sf;
    Register rd = (Register)i->data.immediate_arithmetic.rd;
    Register rn = (Register)i->data.immediate_arithmetic.rn;
    uint64 op2 = i->data.immediate_arithmetic.imm12;

    // Shifting
    if (i->data.immediate_arithmetic.sh) {
        op2 = op2 << 12;
    }

    // Initial value
    uint64 val_n = sf ? read_reg_64(state, rn) : (uint64)read_reg_32(state, rn);

    // Determine operation
    OpType op = i->op_type;
    bool is_sub = (op == OP_TYPE_SUB || op == OP_TYPE_SUBS);
    bool set_flags = (op == OP_TYPE_ADDS || op == OP_TYPE_SUBS);

    // Call handler
    process_arithmetic(state, rd, val_n, op2, is_sub, set_flags, sf);
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

void execute_wide_move(State *state, Instruction *i) {
    // Unpack
    bool sf = i->data.wide_move.sf;
    Register rd = i->data.wide_move.rd;
    uint64 imm16 = (uint64)i->data.wide_move.imm16;
    int shift = i->data.wide_move.hw * 16;
    OpType op = i->op_type;

    // Compute values
    uint64 shifted_imm = imm16 << shift;

    // Logic
    uint64 result = 0;
    switch (op) {
        case OP_TYPE_MOVZ:
            result = shifted_imm;
            break;
        case OP_TYPE_MOVN:
            result = ~shifted_imm;
            break;
        case OP_TYPE_MOVK: {
            uint64 keep_mask = ~(BITMASK_LOWER_16_BITS << shift);
            uint64 existing_value = sf ? read_reg_64(state, rd) : (uint64)read_reg_32(state, rd);
            result = (existing_value & keep_mask) | shifted_imm;
            break;
        }
        default:
            break;
    }
    result = sf ? result : (uint32)result;

    // Storing
    if (sf) {
        write_reg_64(state, rd, result);
    } else {
        write_reg_32(state, rd, result);
    }

}

// Used in `execute_single_data_transfer` to calculate the source address in the load/store.
uint64 find_address(State* state, SingleDataTransfer data_transfer) {
    uint64 xn = read_reg_64(state, data_transfer.xn);

    switch (data_transfer.mode) {
        case ADDR_UNSIGNED_OFFSET: {
            uint64 uoffset = data_transfer.sf ? (uint64)data_transfer.offset * 8 : (uint64)data_transfer.offset * 4;
            return xn + uoffset;
        }
        case ADDR_REGISTER_OFFSET: {
            uint64 xm = read_reg_64(state, data_transfer.xm);
            return xn + xm;
        }
        case ADDR_PRE_INDEXED: {
            return xn + data_transfer.offset;
        }
        case ADDR_POST_INDEXED: {
            return xn;
        }
        default:
            printf("Error: addressing mode not recognized.\n");
            exit(EXIT_FAILURE);
    }
}

// PRE: op == OP_TYPE_SINGLE_DATA_TRANSFER
void execute_single_data_transfer(State* state, Instruction* i) {
    SingleDataTransfer data_transfer = i->data.single_data_transfer;
    uint64 address = find_address(state, data_transfer);
    int rt = i->data.single_data_transfer.rt;

    if (data_transfer.L) { // Load operation
        if (data_transfer.sf) {
            uint64 data = read_mem_64(state, address);
            write_reg_64(state, rt, data);
        } else {
            uint32 data = read_mem_32(state, address);
            write_reg_32(state, rt, data);
        }
    } else { // Store operation
        if (data_transfer.sf) {
            uint64 data = read_reg_64(state, rt);
            write_mem_64(state, address, data);
        } else {
            uint32 data = read_reg_32(state, rt);
            write_mem_32(state, address, data);
        }
    }

    // Write-back for pre/post-indexed
    if (data_transfer.mode == ADDR_PRE_INDEXED || data_transfer.mode == ADDR_POST_INDEXED) {
        uint64 old_xn = read_reg_64(state, data_transfer.xn);
        write_reg_64(state, data_transfer.xn, old_xn + data_transfer.offset);
    }
}

void execute_load_literal(State* state, Instruction* i) {
    LoadLiteral instruction_data = i->data.load_literal;
    uint64 transfer_address = state->PC + instruction_data.simm19 * 4;
    if (instruction_data.sf) {
        uint64 data = read_mem_64(state, transfer_address);
        write_reg_64(state, instruction_data.rt, data);
    } else {
        uint32 data = read_mem_32(state, transfer_address);
        write_reg_32(state, instruction_data.rt, data);
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
        // Halt instruction
        case OP_TYPE_HALT:
            return true;

        // Data processing instructions (immediate)
        case OP_TYPE_ADD:
        case OP_TYPE_ADDS:
        case OP_TYPE_SUB:
        case OP_TYPE_SUBS:
            execute_arithmetic_immediate(state, i);
            break;
        case OP_TYPE_MOVN:
        case OP_TYPE_MOVZ:
        case OP_TYPE_MOVK:
            execute_wide_move(state, i);
            break;

        // Data processing instructions (register)
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
        // Single data transfer
        case OP_TYPE_SINGLE_DATA_TRANSFER:
            execute_single_data_transfer(state, i);
            break;
        // Load literal
        case OP_TYPE_LOAD_LITERAL:
            execute_load_literal(state, i);
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

                offset_pc(state, offset);
            } else {
                inc_pc(state);
            }
            break;

        // Unconditional branch
        case OP_TYPE_AL: {
            int64 offset = (int64)i->data.uncond_branch.simm26 * 4;
            offset_pc(state, offset);
            break;
        }


        // Register branch
        case OP_TYPE_BR:
            write_pc(state, read_reg_64(state, i->data.reg_branch.xn));
            break;


        // TEMP until all instructions have been implemented
        default:
            break;
    }

    if (!(op == OP_TYPE_EQ || op == OP_TYPE_NE || op == OP_TYPE_GE || op == OP_TYPE_LT || op == OP_TYPE_GT || op == OP_TYPE_LE || op == OP_TYPE_AL || op == OP_TYPE_BR)) {
        inc_pc(state);
    }

    return false;
}
