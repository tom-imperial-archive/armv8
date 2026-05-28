#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "utils/types.h"
#include "common/instruction.h"
#include "assembler/encode/encode.h"

// HALT INSTRUCTION
void test_encode_halt(void) {
    Instruction i = {
        .op_type = OP_TYPE_HALT,
    };
    assert(encode_instruction(&i) == 0x8A000000);
}

// IMMEDIATE ARITHMETIC
void test_encode_imm_add(void) {
    Instruction i = {
        .op_type = OP_TYPE_ADD,
        .data.immediate_arithmetic = {
            .sf = 1,
            .rn = 1,
            .sh = 1,
            .rd = 0,
            .imm12 = 10,
        }
    };
    assert(encode_instruction(&i) == 0x91402820);
}

void test_encode_imm_adds(void) {
    Instruction i = {
        .op_type = OP_TYPE_ADDS,
        .data.immediate_arithmetic = {
            .sf = 0,
            .rn = 2,
            .sh = 1,
            .rd = 3,
            .imm12 = 15,
        }
    };
    assert(encode_instruction(&i) == 0x31403C43);
}

void test_encode_imm_sub(void) {
    Instruction i = {
        .op_type = OP_TYPE_SUB,
        .data.immediate_arithmetic = {
            .sf = 1,
            .rn = 4,
            .sh = 1,
            .rd = 5,
            .imm12 = 20,
        }
    };
    assert(encode_instruction(&i) == 0xD1405085);
}

void test_encode_imm_subs(void) {
    Instruction i = {
        .op_type = OP_TYPE_SUBS,
        .data.immediate_arithmetic = {
            .sf = 0,
            .rn = 6,
            .sh = 1,
            .rd = 7,
            .imm12 = 25,
        }
    };
    assert(encode_instruction(&i) == 0x714064C7);
}

// 3. WIDE MOVE
void test_encode_wide_movn(void) {
    Instruction i = {
        .op_type = OP_TYPE_MOVN,
        .data.wide_move = {
            .sf = 1,
            .hw = 0,
            .imm16 = 0x1234,
            .rd = 8,
        }
    };
    assert(encode_instruction(&i) == 0x92824688);
}

void test_encode_wide_movz(void) {
    Instruction i = {
        .op_type = OP_TYPE_MOVZ,
        .data.wide_move = {
            .sf = 1,
            .hw = 1,
            .imm16 = 0xABCD,
            .rd = 9,
        }
    };
    assert(encode_instruction(&i) == 0xD2B579A9);
}

void test_encode_wide_movk(void) {
    Instruction i = {
        .op_type = OP_TYPE_MOVK,
        .data.wide_move = {
            .sf = 0,
            .hw = 2,
            .imm16 = 0xFFFF,
            .rd = 10,
        }
    };
    assert(encode_instruction(&i) == 0x72DFFFEA);
}

// 4. REGISTER ARITHMETIC / LOGIC
void test_encode_reg_add(void) {
    Instruction i = {
        .op_type = OP_TYPE_REG_ADD,
        .data.register_arithmetic_logic = {
            .sf = 1,
            .shift = 0,
            .rm = 12,
            .operand = 0,
            .rn = 11,
            .rd = 13,
        }
    };
    assert(encode_instruction(&i) == 0x8B0C016D);
}

void test_encode_reg_adds(void) {
    Instruction i = {
        .op_type = OP_TYPE_REG_ADDS,
        .data.register_arithmetic_logic = {
            .sf = 0,
            .shift = 1,
            .rm = 14,
            .operand = 2,
            .rn = 15,
            .rd = 16,
        }
    };
    assert(encode_instruction(&i) == 0x2B4E09F0);
}

void test_encode_reg_sub(void) {
    Instruction i = {
        .op_type = OP_TYPE_REG_SUB,
        .data.register_arithmetic_logic = {
            .sf = 1,
            .shift = 2,
            .rm = 1,
            .operand = 4,
            .rn = 2,
            .rd = 3,
        }
    };
    assert(encode_instruction(&i) == 0xCB811043);
}

void test_encode_reg_subs(void) {
    Instruction i = {
        .op_type = OP_TYPE_REG_SUBS,
        .data.register_arithmetic_logic = {
            .sf = 0,
            .shift = 0,
            .rm = 4,
            .operand = 0,
            .rn = 5,
            .rd = 6,
        }
    };
    assert(encode_instruction(&i) == 0x6B0400A6);
}

void test_encode_log_and(void) {
    Instruction i = {
        .op_type = OP_TYPE_AND,
        .data.register_arithmetic_logic = {
            .sf = 1,
            .shift = 0,
            .rm = 8,
            .operand = 0,
            .rn = 9,
            .rd = 10,
        }
    };
    assert(encode_instruction(&i) == 0x8A08012A);
}

void test_encode_log_bic(void) {
    Instruction i = {
        .op_type = OP_TYPE_BIC,
        .data.register_arithmetic_logic = {
            .sf = 0,
            .shift = 0,
            .rm = 11,
            .operand = 0,
            .rn = 12,
            .rd = 13,
        }
    };
    assert(encode_instruction(&i) == 0x0A2B018D);
}

void test_encode_log_orr(void) {
    Instruction i = {
        .op_type = OP_TYPE_ORR,
        .data.register_arithmetic_logic = {
            .sf = 1,
            .shift = 0,
            .rm = 14,
            .operand = 0,
            .rn = 15,
            .rd = 16,
        }
    };
    assert(encode_instruction(&i) == 0xAA0E01F0);
}

void test_encode_log_orn(void) {
    Instruction i = {
        .op_type = OP_TYPE_ORN,
        .data.register_arithmetic_logic = {
            .sf = 0,
            .shift = 0,
            .rm = 17,
            .operand = 0,
            .rn = 18,
            .rd = 19,
        }
    };
    assert(encode_instruction(&i) == 0x2A310253);
}

void test_encode_log_eor(void) {
    Instruction i = {
        .op_type = OP_TYPE_EOR,
        .data.register_arithmetic_logic = {
            .sf = 1,
            .shift = 0,
            .rm = 20,
            .operand = 0,
            .rn = 21,
            .rd = 22,
        }
    };
    assert(encode_instruction(&i) == 0xCA1402B6);
}

void test_encode_log_eon(void) {
    Instruction i = {
        .op_type = OP_TYPE_EON,
        .data.register_arithmetic_logic = {
            .sf = 0,
            .shift = 0,
            .rm = 23,
            .operand = 0,
            .rn = 24,
            .rd = 25,
        }
    };
    assert(encode_instruction(&i) == 0x4A370319);
}

void test_encode_log_ands(void) {
    Instruction i = {
        .op_type = OP_TYPE_ANDS,
        .data.register_arithmetic_logic = {
            .sf = 1,
            .shift = 0,
            .rm = 26,
            .operand = 0,
            .rn = 27,
            .rd = 28,
        }
    };
    assert(encode_instruction(&i) == 0xEA1A037C);
}

void test_encode_log_bics(void) {
    Instruction i = {
        .op_type = OP_TYPE_BICS,
        .data.register_arithmetic_logic = {
            .sf = 0,
            .shift = 0,
            .rm = 29,
            .operand = 0,
            .rn = 30,
            .rd = 0,
        }
    };
    assert(encode_instruction(&i) == 0x6A3D03C0);
}

// 5. REGISTER MULTIPLY
void test_encode_madd(void) {
    Instruction i = {
        .op_type = OP_TYPE_MADD,
        .data.multiply = {
            .sf = 1,
            .rm = 1,
            .ra = 2,
            .rn = 3,
            .rd = 4,
        }
    };
    assert(encode_instruction(&i) == 0x9B010864);
}

void test_encode_msub(void) {
    Instruction i = {
        .op_type = OP_TYPE_MSUB,
        .data.multiply = {
            .sf = 0,
            .rm = 5,
            .ra = 6,
            .rn = 7,
            .rd = 8,
        }
    };
    assert(encode_instruction(&i) == 0x1B0598E8);
}

// SINGLE DATA TRANSFER
void test_encode_sdt_load(void) {
    Instruction i = {
        .op_type = OP_TYPE_SINGLE_DATA_TRANSFER,
        .data.single_data_transfer = {
            .sf = 1,
            .U = 1,
            .L = 1,
            .xn = 10,
            .rt = 11,
            .offset = 2,
        }
    };
    assert(encode_instruction(&i) == 0xF940094B);
}

void test_encode_sdt_store(void) {
    Instruction i = {
        .op_type = OP_TYPE_SINGLE_DATA_TRANSFER,
        .data.single_data_transfer = {
            .sf = 0,
            .U = 0,
            .L = 0,
            .xn = 12,
            .rt = 13,
            .offset = 4,
        }
    };
    assert(encode_instruction(&i) == 0xB800118D);
}

// LOAD LITERAL
void test_encode_load_literal(void) {
    Instruction i = {
        .op_type = OP_TYPE_LOAD_LITERAL,
        .data.load_literal = {
            .sf = 1,
            .simm19 = 16,
            .rt = 14,
        }
    };
    assert(encode_instruction(&i) == 0x5800020E);
}

// BRANCH INSTRUCTIONS
void test_encode_branch_eq(void) {
    Instruction i = {
        .op_type = OP_TYPE_EQ,
        .data.cond_branch.simm19 = 8,
    };
    assert(encode_instruction(&i) == 0x54000100);
}

void test_encode_branch_ne(void) {
    Instruction i = {
        .op_type = OP_TYPE_NE,
        .data.cond_branch.simm19 = 12,
    };
    assert(encode_instruction(&i) == 0x54000181);
}

void test_encode_branch_ge(void) {
    Instruction i = {
        .op_type = OP_TYPE_GE,
        .data.cond_branch.simm19 = 16,
    };
    assert(encode_instruction(&i) == 0x5400020A);
}

void test_encode_branch_lt(void) {
    Instruction i = {
        .op_type = OP_TYPE_LT,
        .data.cond_branch.simm19 = 20,
    };
    assert(encode_instruction(&i) == 0x5400028B);
}

void test_encode_branch_gt(void) {
    Instruction i = {
        .op_type = OP_TYPE_GT,
        .data.cond_branch.simm19 = 24,
    };
    assert(encode_instruction(&i) == 0x5400030C);
}

void test_encode_branch_le(void) {
    Instruction i = {
        .op_type = OP_TYPE_LE,
        .data.cond_branch.simm19 = 28,
    };
    assert(encode_instruction(&i) == 0x5400038D);
}

void test_encode_branch_al(void) {
    Instruction i = {
        .op_type = OP_TYPE_AL,
        .data.cond_branch.simm19 = 32,
    };
    assert(encode_instruction(&i) == 0x5400040E);
}

void test_encode_uncond_branch(void) {
    Instruction i = {
        .op_type = OP_TYPE_UNCONDITIONAL_BRANCH,
        .data.uncond_branch.simm26 = 64,
    };
    assert(encode_instruction(&i) == 0x14000040);
}

void test_encode_reg_branch(void) {
    Instruction i = {
        .op_type = OP_TYPE_BR,
        .data.reg_branch.xn = 30,
    };
    assert(encode_instruction(&i) == 0xD61F03C0);
}

// NEGATIVE SIMM TESTS (Two's Complement)

void test_encode_negative_uncond_branch(void) {
    Instruction i = {
        .op_type = OP_TYPE_UNCONDITIONAL_BRANCH,
        .data.uncond_branch.simm26 = -4,
    };
    assert(encode_instruction(&i) == 0x17FFFFFC);
}

void test_encode_negative_cond_branch(void) {
    Instruction i = {
        .op_type = OP_TYPE_EQ,
        .data.cond_branch.simm19 = -2,
    };
    assert(encode_instruction(&i) == 0x54FFFFC0);
}

void test_encode_negative_load_literal(void) {
    Instruction i = {
        .op_type = OP_TYPE_LOAD_LITERAL,
        .data.load_literal = {
            .sf = 1,
            .simm19 = -3,
            .rt = 0,
        }
    };
    assert(encode_instruction(&i) == 0x58FFFFA0);
}

int main(void) {
    printf("Encode Tests\n");
    printf("-------------\n");

    // Halts
    test_encode_halt();

    // Immediate Arithmetic
    test_encode_imm_add();
    test_encode_imm_adds();
    test_encode_imm_sub();
    test_encode_imm_subs();

    // Wide Move
    test_encode_wide_movn();
    test_encode_wide_movz();
    test_encode_wide_movk();

    // Register Arithmetic / Logic
    test_encode_reg_add();
    test_encode_reg_adds();
    test_encode_reg_sub();
    test_encode_reg_subs();
    test_encode_log_and();
    test_encode_log_bic();
    test_encode_log_orr();
    test_encode_log_orn();
    test_encode_log_eor();
    test_encode_log_eon();
    test_encode_log_ands();
    test_encode_log_bics();

    // Register Multiply
    test_encode_madd();
    test_encode_msub();

    // Single Data Transfer
    test_encode_sdt_load();
    test_encode_sdt_store();

    // Load Literal
    test_encode_load_literal();

    // Branches
    test_encode_branch_eq();
    test_encode_branch_ne();
    test_encode_branch_ge();
    test_encode_branch_lt();
    test_encode_branch_gt();
    test_encode_branch_le();
    test_encode_branch_al();
    test_encode_uncond_branch();
    test_encode_reg_branch();

    // Negative SIMM tests
    test_encode_negative_uncond_branch();
    test_encode_negative_cond_branch();
    test_encode_negative_load_literal();

    printf("All tests passed\n");

    return 0;
}
