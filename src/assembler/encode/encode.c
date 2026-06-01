#include <assert.h>
#include "common/error.h"
#include "utils/types.h"
#include "common/instruction.h"

/*
Here, we take an instruction, represented by a struct, and turn it into the actual 32-bit representation.
This will ultimately be achieved by bitwise operations and masks.

Essentially, we will have one big switch, like we did in execute,
    but the work being done is like the opposite of what was done in decode.
*/

bool is_immediate_arithmetic(OpType op_type) {
    return op_type == OP_TYPE_ADD
        || op_type == OP_TYPE_ADDS
        || op_type == OP_TYPE_SUB
        || op_type == OP_TYPE_SUBS;
}

bool is_wide_move(OpType op_type) {
    return op_type == OP_TYPE_MOVN
        || op_type == OP_TYPE_MOVZ
        || op_type == OP_TYPE_MOVK;
}

bool is_register_arithmetic_logic(OpType op_type) {
    return op_type == OP_TYPE_REG_ADD
        || op_type == OP_TYPE_REG_ADDS
        || op_type == OP_TYPE_REG_SUB
        || op_type == OP_TYPE_REG_SUBS
        || op_type == OP_TYPE_AND
        || op_type == OP_TYPE_BIC
        || op_type == OP_TYPE_ORR
        || op_type == OP_TYPE_ORN
        || op_type == OP_TYPE_EOR
        || op_type == OP_TYPE_EON
        || op_type == OP_TYPE_ANDS
        || op_type == OP_TYPE_BICS;
}

bool is_register_multiply(OpType op_type) {
    return op_type == OP_TYPE_MADD || op_type == OP_TYPE_MSUB;
}

bool is_single_data_transfer(OpType op_type) {
    return op_type == OP_TYPE_SINGLE_DATA_TRANSFER;
}

bool is_load_literal(OpType op_type) {
    return op_type == OP_TYPE_LOAD_LITERAL;
}

bool is_conditional_branch(OpType op_type) {
    return op_type == OP_TYPE_EQ
        || op_type == OP_TYPE_NE
        || op_type == OP_TYPE_GE
        || op_type == OP_TYPE_LT
        || op_type == OP_TYPE_GT
        || op_type == OP_TYPE_LE
        || op_type == OP_TYPE_AL;
}

bool is_unconditional_branch(OpType op_type) {
    return op_type == OP_TYPE_UNCONDITIONAL_BRANCH;
}

bool is_register_branch(OpType op_type) {
    return op_type == OP_TYPE_BR;
}

bool is_directive_int(OpType op_type) {
    return op_type == OP_TYPE_DIRECTIVE_INT;
}

const int MASK_OP0_DPII = 0x8; // 0b1000
const int MASK_OP0_DPIR = 0x5; // 0b0101
const int MASK_OP0_LOAD_STORE = 0xC; // 0b1100
const int MASK_OP0_BRANCH = 0xA; // 0b1010
const int OFFSET_OP0 = 25;
const int OFFSET_SF_DPII_DPIR = 31;
const int OFFSET_SF_LOAD_STORE = 30;
const int OFFSET_OPC = 29;
const int OFFSET_OPI = 23;
const int OFFSET_OPR = 21;
const int OFFSET_RM = 16;
const int OFFSET_RN = 5;
const int OFFSET_XN = 5;
const int OFFSET_TOP_BITS = 29;

uint32 encode_immediate_arithmetic(Instruction* i) {
    uint32 instruction = 0;
    ImmediateArithmeticInstruction data = i->data.immediate_arithmetic;

    // OP0
    instruction |= MASK_OP0_DPII << OFFSET_OP0;

    // SF
    if (data.sf) {
        instruction |= 1 << OFFSET_SF_DPII_DPIR;
    }

    // OPC
    const int MASK_ADDS = 0x1; // 0b01
    const int MASK_SUB = 0x2;  // 0b10
    const int MASK_SUBS = 0x3; // 0b11
    switch (i->op_type) {
        // OPC is 0b00 so nothing to do.
        case OP_TYPE_ADD: break;
        case OP_TYPE_ADDS: instruction |= MASK_ADDS << OFFSET_OPC; break;
        case OP_TYPE_SUB:  instruction |= MASK_SUB << OFFSET_OPC;  break;
        case OP_TYPE_SUBS: instruction |= MASK_SUBS << OFFSET_OPC; break;
        default: error(INCORRECT_OP_TYPE, NULL); break;
    }

    // OPI
    const int MASK_OPI_IMMEDIATE_ARITHMETIC = 0x2; // 0b010
    instruction |= MASK_OPI_IMMEDIATE_ARITHMETIC << OFFSET_OPI;

    // SH
    const int OFFSET_SH = 22;
    if (data.sh) {
        instruction |= 1 << OFFSET_SH;
    }

    // IMM12
    const int OFFSET_IMM12 = 10;
    instruction |= data.imm12 << OFFSET_IMM12;

    // RN
    instruction |= data.rn << OFFSET_RN;

    // RD
    instruction |= data.rd;

    return instruction;
}

uint32 encode_wide_move(Instruction* i) {
    WideMoveInstruction data = i->data.wide_move;
    uint32 instruction = 0;

    // OP0
    instruction |= MASK_OP0_DPII << OFFSET_OP0;

    // SF
    if (data.sf) {
        instruction |= 1 << OFFSET_SF_DPII_DPIR;
    }

    // OPC
    const int MASK_MOVZ = 0x2; // 0b10
    const int MASK_MOVK = 0x3; // 0b11
    switch (i->op_type) {
        // OPC is 0b00 so nothing to do.
        case OP_TYPE_MOVN: break;
        case OP_TYPE_MOVZ: instruction |= MASK_MOVZ << OFFSET_OPC; break;
        case OP_TYPE_MOVK: instruction |= MASK_MOVK << OFFSET_OPC; break;
        // Wrong op_type
        default: error(INCORRECT_OP_TYPE, NULL); break;
    }

    // OPI
    const int MASK_OPI_WIDE_MOVE = 0x5; // 0b101
    instruction |= MASK_OPI_WIDE_MOVE << OFFSET_OPI;

    // HW
    const int OFFSET_HW = 21;
    instruction |= data.hw << OFFSET_HW;

    // IMM16
    const int OFFSET_IMM16 = 5;
    // TODO: might need to mask top bits?
    instruction |= data.imm16 << OFFSET_IMM16;

    // RD
    instruction |= data.rd;

    return instruction;
}

// uint32 encode_register_arithmetic_logic(Instruction* i) {
//     uint32 instruction = 0;
//     RegisterArithmeticLogicInstruction data = i->data.register_arithmetic_logic;

//     // OP0
//     instruction |= MASK_OP0_DPIR << OFFSET_OP0;

//     // SF
//     if (data.sf) {
//         instruction |= 1 << OFFSET_SF_DPII_DPIR;
//     }

//     // OPC
//     const int MASK_ADDS = 0x1; // 0b01
//     const int MASK_SUB = 0x2;  // 0b10
//     const int MASK_SUBS = 0x3; // 0b11
//     switch (i->op_type) {
//         // OPC is 0b00
//         case OP_TYPE_REG_ADD:
//         case OP_TYPE_AND:
//         case OP_TYPE_BIC:
//             break;

//         case OP_TYPE_REG_ADDS:
//         case OP_TYPE_ORR:
//         case OP_TYPE_ORN:
//             instruction |= MASK_ADDS << OFFSET_OPC; // 0b01
//             break;

//         case OP_TYPE_REG_SUB:
//         case OP_TYPE_EOR:
//         case OP_TYPE_EON:
//             instruction |= MASK_SUB << OFFSET_OPC; // 0b10
//             break;

//         case OP_TYPE_REG_SUBS:
//         case OP_TYPE_ANDS:
//         case OP_TYPE_BICS:
//             instruction |= MASK_SUBS << OFFSET_OPC; // 0b11
//             break;

//         // Wrong op_type
//         default: assert(false); break;
//     }

//     // M
//     // Only set for multiply

//     // OPR
//     const int MASK_ARITHMETIC_OPR = 0x8; // 0b1000
//     const int OFFSET_SHIFT = 22;
//     instruction |= MASK_ARITHMETIC_OPR << OFFSET_OPR;
//     instruction |= data.shift << OFFSET_SHIFT;

//     // RM
//     instruction |= data.rm << OFFSET_RM;

//     // OPERAND
//     const int OFFSET_OPERAND = 10;
//     instruction |= data.operand << OFFSET_OPERAND;

//     // RN
//     instruction |= data.rn << OFFSET_RN;

//     // RD
//     instruction |= data.rd;

//     return instruction;
// }

uint32 encode_register_arithmetic_logic(Instruction* i) {
    uint32 instruction = 0;
    RegisterArithmeticLogicInstruction data = i->data.register_arithmetic_logic;

    // OP0
    instruction |= MASK_OP0_DPIR << OFFSET_OP0;

    // SF
    if (data.sf) {
        instruction |= 1 << OFFSET_SF_DPII_DPIR;
    }

    // M is 0 for Arithmetic/Logical

    // OPC (Bits 30-29) and OPR (Bits 24-21) setup
    // Arithmetic OPR: Bit 24 = 1, Bits 23-22 = shift, Bit 21 = 0
    // Logical OPR:    Bit 24 = 0, Bits 23-22 = shift, Bit 21 = N
    uint32 opc = 0;
    uint32 is_logical = 0;
    uint32 n_bit = 0;

    switch (i->op_type) {
        // ARITHMETIC
        case OP_TYPE_REG_ADD:  opc = 0x0; break;
        case OP_TYPE_REG_ADDS: opc = 0x1; break;
        case OP_TYPE_REG_SUB:  opc = 0x2; break;
        case OP_TYPE_REG_SUBS: opc = 0x3; break;

        // LOGICAL
        case OP_TYPE_AND:  opc = 0x0; is_logical = 1; n_bit = 0; break;
        case OP_TYPE_BIC:  opc = 0x0; is_logical = 1; n_bit = 1; break;
        case OP_TYPE_ORR:  opc = 0x1; is_logical = 1; n_bit = 0; break;
        case OP_TYPE_ORN:  opc = 0x1; is_logical = 1; n_bit = 1; break;
        case OP_TYPE_EOR:  opc = 0x2; is_logical = 1; n_bit = 0; break;
        case OP_TYPE_EON:  opc = 0x2; is_logical = 1; n_bit = 1; break;
        case OP_TYPE_ANDS: opc = 0x3; is_logical = 1; n_bit = 0; break;
        case OP_TYPE_BICS: opc = 0x3; is_logical = 1; n_bit = 1; break;

        default: assert(false); break;
    }

    // Apply OPC
    instruction |= opc << 29;

    // Apply OPR (Bits 24-21)
    uint32 opr = 0;
    if (!is_logical) {
        // Arithmetic: (1 << 3) | (shift << 1) | 0
        opr = (1 << 3) | (data.shift << 1);
    } else {
        // Logical: (0 << 3) | (shift << 1) | N
        opr = (data.shift << 1) | n_bit;
    }
    instruction |= opr << 21;

    // RM (Bits 20-16)
    instruction |= data.rm << 16;

    // OPERAND (Bits 15-10) -> Was incorrectly set to 9!
    instruction |= data.operand << 10;

    // RN (Bits 9-5)
    instruction |= data.rn << 5;

    // RD (Bits 4-0)
    instruction |= data.rd;

    return instruction;
}

uint32 encode_register_multiply(Instruction* i) {
    uint32 instruction = 0;
    RegisterMultiplyInstruction data = i->data.multiply;

    // OP0
    instruction |= MASK_OP0_DPIR << OFFSET_OP0;

    // SF
    if (data.sf) {
        instruction |= 1 << OFFSET_SF_DPII_DPIR;
    }

    // OPC
    // Always set to 0b00

    // M
    const int OFFSET_M = 28;
    instruction |= 1 << OFFSET_M;

    // OPR
    const int MASK_OPR_REGISTER_MULTIPLY = 0x8;
    instruction |= MASK_OPR_REGISTER_MULTIPLY << OFFSET_OPR;

    // RM
    instruction |= data.rm << OFFSET_RM;

    // X
    const int OFFSET_X = 15;
    if (i->op_type == OP_TYPE_MSUB) {
        instruction |= 1 << OFFSET_X;
    }

    // RA
    const int OFFSET_RA = 10;
    instruction |= data.ra << OFFSET_RA;

    // RN
    instruction |= data.rn << OFFSET_RN;

    // RD
    instruction |= data.rd;

    return instruction;
}

uint32 encode_single_data_transfer(Instruction* i) {
    uint32 instruction = 0;
    SingleDataTransfer data = i->data.single_data_transfer;

    // Top bits
    const int MASK_TOP_BITS = 0x5; // 0b101
    instruction |= MASK_TOP_BITS << OFFSET_TOP_BITS;

    // OP0
    instruction |= MASK_OP0_LOAD_STORE << OFFSET_OP0;

    // SF
    if (data.sf) {
        instruction |= 1 << OFFSET_SF_LOAD_STORE;
    }

    // L
    const int OFFSET_L = 22;
    if (data.L) {
        instruction |= 1 << OFFSET_L;
    }

    // Mode-specific encoding
    const int OFFSET_U = 24;

    if (data.mode == ADDR_UNSIGNED_OFFSET) {
        // U = 1
        instruction |= 1 << OFFSET_U;
        instruction |= (data.offset & 0xFFF) << 10;

    } else if (data.mode == ADDR_REGISTER_OFFSET) {
        // U = 0
        // Bit 21 = 1 for register offset mode
        instruction |= 1 << 21;
        // Rm
        instruction |= (data.xm & 0x1F) << 16;
        // Option
        instruction |= 0x1A << 10;

    } else {
        // ADDR_PRE_INDEXED or ADDR_POST_INDEXED
        // U = 0, Bit 21 = 0

        // simm9
        instruction |= (data.offset & 0x1FF) << 12;

        // i-bit
        int i_bit = (data.mode == ADDR_PRE_INDEXED) ? 1 : 0;
        instruction |= i_bit << 11;

        // Bit 10 is always 1 for indexed modes
        instruction |= 1 << 10;
    }

    // XN
    instruction |= data.xn << OFFSET_XN;

    // RT
    instruction |= data.rt;

    return instruction;
}

uint32 encode_load_literal(Instruction* i) {
    uint32 instruction = 0;
    LoadLiteral data = i->data.load_literal;

    // OP0
    instruction |= MASK_OP0_LOAD_STORE << OFFSET_OP0;

    // SF
    if (data.sf) {
        instruction |= 1 << OFFSET_SF_LOAD_STORE;
    }

    // SIMM19
    const int MASK_LOWER_19_BITS = 0x7FFFF;
    const int OFFSET_SIMM19 = 5;
    instruction |= (data.simm19 & MASK_LOWER_19_BITS) << OFFSET_SIMM19;

    // RT
    instruction |= data.rt;

    return instruction;
}

uint32 encode_conditional_branch(Instruction* i) {
    uint32 instruction = 0;
    ConditionalBranchInstruction data = i->data.cond_branch;

    // Top bits
    const int MASK_TOP_BITS_CONDITIONAL_BRANCH = 0x2; // 0b010
    instruction |= MASK_TOP_BITS_CONDITIONAL_BRANCH << OFFSET_TOP_BITS;

    // OP0
    instruction |= MASK_OP0_BRANCH << OFFSET_OP0;

    // SIMM19
    const int MASK_LOWER_19_BITS = 0x7FFFF;
    const int OFFSET_SIMM19 = 5;
    instruction |= (data.simm19 & MASK_LOWER_19_BITS) << OFFSET_SIMM19;

    // COND
    const int MASK_COND_NE = 0x1; // 0b0001
    const int MASK_COND_GE = 0xA; // 0b1010
    const int MASK_COND_LT = 0xB; // 0b1011
    const int MASK_COND_GT = 0xC; // 0b1100
    const int MASK_COND_LE = 0xD; // 0b1101
    const int MASK_COND_AL = 0xE; // 0b1110
    switch (i->op_type) {
        // Mask is 0b00.
        case OP_TYPE_EQ: break;
        case OP_TYPE_NE: instruction |= MASK_COND_NE; break;
        case OP_TYPE_GE: instruction |= MASK_COND_GE; break;
        case OP_TYPE_LT: instruction |= MASK_COND_LT; break;
        case OP_TYPE_GT: instruction |= MASK_COND_GT; break;
        case OP_TYPE_LE: instruction |= MASK_COND_LE; break;
        case OP_TYPE_AL: instruction |= MASK_COND_AL; break;
        // Wrong op_type
        default: error(INCORRECT_OP_TYPE, NULL); break;
    }

    return instruction;
}

uint32 encode_unconditional_branch(Instruction* i) {
    uint32 instruction = 0;
    UnconditionalBranchInstruction data = i->data.uncond_branch;

    // OP0
    instruction |= MASK_OP0_BRANCH << OFFSET_OP0;

    // SIMM26
    const int MASK_LOWER_26_BITS = 0x03FFFFFF;
    instruction |= data.simm26 & MASK_LOWER_26_BITS;

    return instruction;
}

uint32 encode_register_branch(Instruction* i) {
    uint32 instruction = 0;
    RegisterBranchInstruction data = i->data.reg_branch;

    // Top bits
    // const int MASK_TOP_BITS_CONDITIONAL_BRANCH = 0x6; // 0b110
    // instruction |= MASK_TOP_BITS_CONDITIONAL_BRANCH << OFFSET_TOP_BITS;

    // // Middle bits
    // const int MASK_MIDDLE_BITS = 0x1F; // 0b111111
    // const int OFFSET_MIDDLE_BITS = 15;
    // instruction |= MASK_MIDDLE_BITS << OFFSET_MIDDLE_BITS;

    const int MASK_UPPER_BITS = 0xD61F0000;
    instruction |= MASK_UPPER_BITS;

    // XN
    instruction |= data.xn << OFFSET_XN;

    return instruction;
}

uint32 encode_directive_int(Instruction* i) {
    return (uint32)i->data.directive_int.value;
}

// Takes a fully populated instruction struct and packs it into a 32 bit binary instruction
// This is the exact opposite of what we did in decode
uint32 encode_instruction(Instruction* i) {
    if (i->op_type == OP_TYPE_HALT) {
        return 0x8A000000;
    } else if (is_immediate_arithmetic(i->op_type)) {
        return encode_immediate_arithmetic(i);
    } else if (is_wide_move(i->op_type)) {
        return encode_wide_move(i);
    } else if (is_register_arithmetic_logic(i->op_type)) {
        return encode_register_arithmetic_logic(i);
    } else if (is_register_multiply(i->op_type)) {
        return encode_register_multiply(i);
    } else if (is_single_data_transfer(i->op_type)) {
        return encode_single_data_transfer(i);
    } else if (is_load_literal(i->op_type)) {
        return encode_load_literal(i);
    } else if (is_conditional_branch(i->op_type)) {
        return encode_conditional_branch(i);
    } else if (is_unconditional_branch(i->op_type)) {
        return encode_unconditional_branch(i);
    } else if (is_register_branch(i->op_type)) {
        return encode_register_branch(i);
    } else if (is_directive_int(i->op_type)) {
        return encode_directive_int(i);
    } else {
        // ERROR: Unrecognized instruction type
        error(UNRECOGNISED_INSTRUCTION_TYPE, NULL);
        return 0;
    }
}
