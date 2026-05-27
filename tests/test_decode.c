#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "emulator/decode.h"

void test_decode_halt(void) {
    Instruction inst;
    DecodeResult res;

    // HALT instruction
    res = decode(0x8A000000, &inst);
    assert(res == DECODE_SUCCESS);
    assert(inst.op_type == OP_TYPE_HALT);
}

void test_decode_dpii(void) {
    Instruction inst;
    DecodeResult res;

    // ADD (Immediate Arithmetic)
    // sf=1, opc=0 (ADD), opi=2, sh=0, imm12=0x123, rn=1, rd=2
    res = decode(0x91048C22, &inst);
    assert(res == DECODE_SUCCESS);
    assert(inst.op_type == OP_TYPE_ADD);
    assert(inst.data.immediate_arithmetic.sf == true);
    assert(inst.data.immediate_arithmetic.sh == 0);
    assert(inst.data.immediate_arithmetic.imm12 == 0x123);
    assert(inst.data.immediate_arithmetic.rn == 1);
    assert(inst.data.immediate_arithmetic.rd == 2);

    // SUBS (Immediate Arithmetic)
    // sf=0, opc=3 (SUBS), opi=2, sh=1, imm12=0xFF, rn=3, rd=4
    res = decode(0x7143FC64, &inst);
    assert(res == DECODE_SUCCESS);
    assert(inst.op_type == OP_TYPE_SUBS);
    assert(inst.data.immediate_arithmetic.sf == false);
    assert(inst.data.immediate_arithmetic.sh == 1);
    assert(inst.data.immediate_arithmetic.imm12 == 0xFF);
    assert(inst.data.immediate_arithmetic.rn == 3);
    assert(inst.data.immediate_arithmetic.rd == 4);

    // MOVZ (Wide Move)
    // sf=1, opc=2 (MOVZ), opi=5, hw=1, imm16=0xABCD, rd=5
    res = decode(0xD2B579A5, &inst);
    assert(res == DECODE_SUCCESS);
    assert(inst.op_type == OP_TYPE_MOVZ);
    assert(inst.data.wide_move.sf == true);
    assert(inst.data.wide_move.hw == 1);
    assert(inst.data.wide_move.imm16 == 0xABCD);
    assert(inst.data.wide_move.rd == 5);
}

void test_decode_dpir(void) {
    Instruction inst;
    DecodeResult res;

    // MADD (Multiply-Add)
    // sf=1, opc=0, M=1, opr=1000, rm=2, x=0, ra=3, rn=4, rd=5
    res = decode(0x9B020C85, &inst);
    assert(res == DECODE_SUCCESS);
    assert(inst.op_type == OP_TYPE_MADD);
    assert(inst.data.multiply.sf == true);
    assert(inst.data.multiply.rm == 2);
    assert(inst.data.multiply.ra == 3);
    assert(inst.data.multiply.rn == 4);
    assert(inst.data.multiply.rd == 5);

    // AND (Logical Register)
    // sf=1, opc=0(AND), M=0, opr=0000(LSL, N=0), rm=2, operand(shift_amt)=6, rn=4, rd=5
    res = decode(0x8A021885, &inst);
    assert(res == DECODE_SUCCESS);
    assert(inst.op_type == OP_TYPE_AND);
    assert(inst.data.register_arithmetic_logic.sf == true);
    assert(inst.data.register_arithmetic_logic.shift == SHIFT_LSL);
    assert(inst.data.register_arithmetic_logic.rm == 2);
    assert(inst.data.register_arithmetic_logic.operand == 6);
    assert(inst.data.register_arithmetic_logic.rn == 4);
    assert(inst.data.register_arithmetic_logic.rd == 5);

    // ADD (Arithmetic Register)
    // sf=0, opc=0(ADD), M=0, opr=1010(LSR, N=0), rm=3, operand(shift_amt)=7, rn=4, rd=5
    res = decode(0x0B431C85, &inst);
    assert(res == DECODE_SUCCESS);
    assert(inst.op_type == OP_TYPE_ADD);
    assert(inst.data.register_arithmetic_logic.sf == false);
    assert(inst.data.register_arithmetic_logic.shift == SHIFT_LSR);
    assert(inst.data.register_arithmetic_logic.rm == 3);
    assert(inst.data.register_arithmetic_logic.operand == 7);
    assert(inst.data.register_arithmetic_logic.rn == 4);
    assert(inst.data.register_arithmetic_logic.rd == 5);
}

void test_decode_sdt(void) {
    Instruction inst;
    DecodeResult res;

    // LDR (Single Data Transfer - Unsigned Offset)
    // sf=1, U=1, L=1(LDR), offset=0x12, xn=2, rt=3
    res = decode(0xF9404843, &inst);
    assert(res == DECODE_SUCCESS);
    assert(inst.op_type == OP_TYPE_SINGLE_DATA_TRANSFER);
    assert(inst.data.single_data_transfer.sf == true);
    assert(inst.data.single_data_transfer.U == true);
    assert(inst.data.single_data_transfer.L == true);
    assert(inst.data.single_data_transfer.offset == 0x12);
    assert(inst.data.single_data_transfer.xn == 2);
    assert(inst.data.single_data_transfer.rt == 3);

    // Load Literal
    // sf=1, simm19=0x1A, rt=4
    res = decode(0x58000344, &inst);
    assert(res == DECODE_SUCCESS);
    assert(inst.op_type == OP_TYPE_LOAD_LITERAL);
    assert(inst.data.load_literal.sf == true);
    assert(inst.data.load_literal.simm19 == 0x1A);
    assert(inst.data.load_literal.rt == 4);
}

void test_decode_branch(void) {
    Instruction inst;
    DecodeResult res;

    // BR (Register Branch)
    // xn=2
    res = decode(0xD6000040, &inst);
    assert(res == DECODE_SUCCESS);
    assert(inst.op_type == OP_TYPE_BR);
    assert(inst.data.reg_branch.xn == 2);

    // B.cond (Conditional Branch)
    // simm19=0x1B, cond=0 (EQ)
    res = decode(0x54000360, &inst);
    assert(res == DECODE_SUCCESS);
    assert(inst.op_type == OP_TYPE_EQ);
    assert(inst.data.cond_branch.simm19 == 0x1B);

    // B (Unconditional Branch)
    // simm26=0x12345
    res = decode(0x14012345, &inst);
    assert(res == DECODE_SUCCESS);
    assert(inst.op_type == OP_TYPE_AL); // Note: decode.c currently maps this to OP_TYPE_AL
    assert(inst.data.uncond_branch.simm26 == 0x12345);
}

void test_decode_negative_simm(void) {
    Instruction inst;
    DecodeResult res;

    // B (Unconditional Branch) with negative offset
    // Branching backwards by 4 words (-16 bytes).
    // -4 in 26-bit two's complement is 0x3FFFFFC.
    // Base opcode: 0x14000000. Combined: 0x14000000 | 0x03FFFFFC = 0x17FFFFFC
    res = decode(0x17FFFFFC, &inst);
    assert(res == DECODE_SUCCESS);
    assert(inst.op_type == OP_TYPE_AL); // Per your decode.c mapping
    assert(inst.data.uncond_branch.simm26 == 0x3FFFFFC);

    // B.cond (Conditional Branch) with negative offset
    // Branching backwards by 8 words (-32 bytes).
    // -8 in 19-bit two's complement is 0x7FFF8.
    // Base opcode: 0x54000000. cond = 1 (NE).
    // Combined: 0x54000000 | (0x7FFF8 << 5) | 0x1 = 0x54FFF001
    res = decode(0x54FFF001, &inst);
    assert(res == DECODE_SUCCESS);
    assert(inst.op_type == OP_TYPE_NE);
    assert(inst.data.cond_branch.simm19 == 0x7FFF8);

    // Load Literal with negative offset
    // Loading from PC - 12 words (-48 bytes).
    // -12 in 19-bit two's complement is 0x7FFF4.
    // Base opcode: 0x58000000. sf = 1 (64-bit). rt = 0.
    // Combined: 0x58000000 | (0x7FFF4 << 5) | 0x0 = 0x58FFE800
    res = decode(0x58FFE800, &inst);
    assert(res == DECODE_SUCCESS);
    assert(inst.op_type == OP_TYPE_LOAD_LITERAL);
    assert(inst.data.load_literal.sf == true);
    assert(inst.data.load_literal.rt == 0);
    assert(inst.data.load_literal.simm19 == 0x7FFF4);
}

void test_decode(void) {
    printf("Decode Tests\n");
    printf("------------\n");

    printf("Running test_decode_halt... ");
    test_decode_halt();
    printf("PASSED\n");

    printf("Running test_decode_dpii... ");
    test_decode_dpii();
    printf("PASSED\n");

    printf("Running test_decode_dpir... ");
    test_decode_dpir();
    printf("PASSED\n");

    printf("Running test_decode_sdt... ");
    test_decode_sdt();
    printf("PASSED\n");

    printf("Running test_decode_branch... ");
    test_decode_branch();
    printf("PASSED\n");

    printf("Running test_decode_negative_simm... ");
    test_decode_negative_simm();
    printf("PASSED\n");

    printf("------------\n");
    printf("All decode tests passed!\n");
}
