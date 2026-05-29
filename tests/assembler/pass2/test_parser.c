#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "common/instruction.h"
#include "assembler/pass2/parser.h"

void test_parse_line_ignores_labels_and_blanks(void) {
    Instruction i;

    char label_line[] = "my_loop:";
    assert(parse_line(label_line, &i, NULL, 0) == false);

    char blank_line[] = "   \n";
    assert(parse_line(blank_line, &i, NULL, 0) == false);

    printf("Test parse line ignores labels and blanks: OK\n");
}

void test_parse_add_immediate_standard(void) {
    Instruction i;
    char line[] = "add x0, x1, #42";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_ADD);
    assert(i.data.immediate_arithmetic.rd == 0);
    assert(i.data.immediate_arithmetic.rn == 1);
    assert(i.data.immediate_arithmetic.sf == true);
    assert(i.data.immediate_arithmetic.imm12 == 42);
    assert(i.data.immediate_arithmetic.sh == 0);

    printf("Test parse add immediate standard: OK\n");
}

void test_parse_sub_immediate_shifted(void) {
    Instruction i;
    char line[] = "sub w2, w3, #10, lsl #12";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_SUB);
    assert(i.data.immediate_arithmetic.rd == 2);
    assert(i.data.immediate_arithmetic.rn == 3);
    assert(i.data.immediate_arithmetic.sf == false);
    assert(i.data.immediate_arithmetic.imm12 == 10);
    assert(i.data.immediate_arithmetic.sh == 1);

    printf("Test parse sub immediate shifted: OK\n");
}

void test_parse_adds_register_standard(void) {
    Instruction i;
    char line[] = "adds x5, x6, x7";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_REG_ADDS);
    assert(i.data.register_arithmetic_logic.rd == 5);
    assert(i.data.register_arithmetic_logic.rn == 6);
    assert(i.data.register_arithmetic_logic.rm == 7);
    assert(i.data.register_arithmetic_logic.sf == true);

    assert(i.data.register_arithmetic_logic.shift == SHIFT_LSL);
    assert(i.data.register_arithmetic_logic.operand == 0);

    printf("Test parse adds register standard: OK\n");
}

void test_parse_subs_register_shifted(void) {
    Instruction i;
    char line[] = "subs w10, w11, w12, asr #4";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_REG_SUBS);
    assert(i.data.register_arithmetic_logic.rd == 10);
    assert(i.data.register_arithmetic_logic.rn == 11);
    assert(i.data.register_arithmetic_logic.rm == 12);
    assert(i.data.register_arithmetic_logic.sf == false);
    assert(i.data.register_arithmetic_logic.shift == SHIFT_ASR);
    assert(i.data.register_arithmetic_logic.operand == 4);

    printf("Test parse subs register shifted: OK\n");
}

void test_parse_cmp_immediate(void) {
    Instruction i;

    char line[] = "cmp x5, #100";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_SUBS);
    assert(i.data.immediate_arithmetic.rd == 31);
    assert(i.data.immediate_arithmetic.rn == 5);
    assert(i.data.immediate_arithmetic.sf == true);
    assert(i.data.immediate_arithmetic.imm12 == 100);
    assert(i.data.immediate_arithmetic.sh == 0);

    printf("Test parse cmp immediate: OK\n");
}

void test_parse_cmn_register_shifted(void) {
    Instruction i;

    char line[] = "cmn w10, w11, lsl #2";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_REG_ADDS);
    assert(i.data.register_arithmetic_logic.rd == 31);
    assert(i.data.register_arithmetic_logic.rn == 10);
    assert(i.data.register_arithmetic_logic.sf == false);
    assert(i.data.register_arithmetic_logic.rm == 11);
    assert(i.data.register_arithmetic_logic.shift == SHIFT_LSL);
    assert(i.data.register_arithmetic_logic.operand == 2);

    printf("Test parse cmn register shifted: OK\n");
}

void test_parse_neg_register(void) {
    Instruction i;

    char line[] = "neg x2, x3";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_REG_SUB);
    assert(i.data.register_arithmetic_logic.rd == 2);
    assert(i.data.register_arithmetic_logic.rn == 31);
    assert(i.data.register_arithmetic_logic.sf == true);
    assert(i.data.register_arithmetic_logic.rm == 3);
    assert(i.data.register_arithmetic_logic.shift == SHIFT_LSL);
    assert(i.data.register_arithmetic_logic.operand == 0);

    printf("Test parse neg register: OK\n");
}

void test_parse_negs_register_shifted(void) {
    Instruction i;

    char line[] = "negs w4, w5, asr #1";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_REG_SUBS);
    assert(i.data.register_arithmetic_logic.rd == 4);
    assert(i.data.register_arithmetic_logic.rn == 31);
    assert(i.data.register_arithmetic_logic.sf == false);
    assert(i.data.register_arithmetic_logic.rm == 5);
    assert(i.data.register_arithmetic_logic.shift == SHIFT_ASR);
    assert(i.data.register_arithmetic_logic.operand == 1);

    printf("Test parse negs register shifted: OK\n");
}

void test_parse_and_register_standard(void) {
    Instruction i;
    char line[] = "and x1, x2, x3";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_AND);
    assert(i.data.register_arithmetic_logic.rd == 1);
    assert(i.data.register_arithmetic_logic.rn == 2);
    assert(i.data.register_arithmetic_logic.rm == 3);
    assert(i.data.register_arithmetic_logic.sf == true); // 64-bit 'x' registers
    assert(i.data.register_arithmetic_logic.shift == SHIFT_LSL); // Default shift
    assert(i.data.register_arithmetic_logic.operand == 0);

    printf("Test parse and register standard: OK\n");
}

void test_parse_orr_register_shifted(void) {
    Instruction i;
    char line[] = "orr w4, w5, w6, lsl #2";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_ORR);
    assert(i.data.register_arithmetic_logic.rd == 4);
    assert(i.data.register_arithmetic_logic.rn == 5);
    assert(i.data.register_arithmetic_logic.rm == 6);
    assert(i.data.register_arithmetic_logic.sf == false); // 32-bit 'w' registers
    assert(i.data.register_arithmetic_logic.shift == SHIFT_LSL);
    assert(i.data.register_arithmetic_logic.operand == 2);

    printf("Test parse orr register shifted: OK\n");
}

void test_parse_eor_register_shifted(void) {
    Instruction i;
    char line[] = "eor x7, x8, x9, asr #4";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_EOR);
    assert(i.data.register_arithmetic_logic.rd == 7);
    assert(i.data.register_arithmetic_logic.rn == 8);
    assert(i.data.register_arithmetic_logic.rm == 9);
    assert(i.data.register_arithmetic_logic.sf == true);
    assert(i.data.register_arithmetic_logic.shift == SHIFT_ASR);
    assert(i.data.register_arithmetic_logic.operand == 4);

    printf("Test parse eor register shifted: OK\n");
}

void test_parse_bics_register_standard(void) {
    Instruction i;
    char line[] = "bics w10, w11, w12";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_BICS);
    assert(i.data.register_arithmetic_logic.rd == 10);
    assert(i.data.register_arithmetic_logic.rn == 11);
    assert(i.data.register_arithmetic_logic.rm == 12);
    assert(i.data.register_arithmetic_logic.sf == false);
    assert(i.data.register_arithmetic_logic.shift == SHIFT_LSL);
    assert(i.data.register_arithmetic_logic.operand == 0);

    printf("Test parse bics register standard: OK\n");
}

void test_parse_tst_register(void) {
    Instruction i;
    char line[] = "tst x1, x2";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_ANDS);
    assert(i.data.register_arithmetic_logic.rd == 31);
    assert(i.data.register_arithmetic_logic.rn == 1);
    assert(i.data.register_arithmetic_logic.rm == 2);
    assert(i.data.register_arithmetic_logic.sf == true);
    assert(i.data.register_arithmetic_logic.shift == SHIFT_LSL);
    assert(i.data.register_arithmetic_logic.operand == 0);

    printf("Test parse tst register: OK\n");
}

void test_parse_mvn_register_shifted(void) {
    Instruction i;

    char line[] = "mvn w3, w4, asr #2";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_ORN);
    assert(i.data.register_arithmetic_logic.rd == 3);
    assert(i.data.register_arithmetic_logic.rn == 31);
    assert(i.data.register_arithmetic_logic.rm == 4);
    assert(i.data.register_arithmetic_logic.sf == false);
    assert(i.data.register_arithmetic_logic.shift == SHIFT_ASR);
    assert(i.data.register_arithmetic_logic.operand == 2);

    printf("Test parse mvn register shifted: OK\n");
}

void test_parse_mov_register(void) {
    Instruction i;

    char line[] = "mov x5, x6";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_ORR);
    assert(i.data.register_arithmetic_logic.rd == 5);
    assert(i.data.register_arithmetic_logic.rn == 31);
    assert(i.data.register_arithmetic_logic.rm == 6);
    assert(i.data.register_arithmetic_logic.sf == true);
    assert(i.data.register_arithmetic_logic.shift == SHIFT_LSL);
    assert(i.data.register_arithmetic_logic.operand == 0);

    printf("Test parse mov register: OK\n");
}

void test_parse_madd_standard(void) {
    Instruction i;
    char line[] = "madd x0, x1, x2, x3";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_MADD);
    assert(i.data.multiply.rd == 0);
    assert(i.data.multiply.rn == 1);
    assert(i.data.multiply.rm == 2);
    assert(i.data.multiply.ra == 3);
    assert(i.data.multiply.sf == true);

    printf("Test parse madd standard: OK\n");
}

void test_parse_msub_standard(void) {
    Instruction i;
    char line[] = "msub w4, w5, w6, w7";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_MSUB);
    assert(i.data.multiply.rd == 4);
    assert(i.data.multiply.rn == 5);
    assert(i.data.multiply.rm == 6);
    assert(i.data.multiply.ra == 7);
    assert(i.data.multiply.sf == false);

    printf("Test parse msub standard: OK\n");
}

void test_parse_mul_alias(void) {
    Instruction i;

    char line[] = "mul x8, x9, x10";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_MADD);
    assert(i.data.multiply.rd == 8);
    assert(i.data.multiply.rn == 9);
    assert(i.data.multiply.rm == 10);
    assert(i.data.multiply.ra == 31);
    assert(i.data.multiply.sf == true);

    printf("Test parse mul alias: OK\n");
}

void test_parse_mneg_alias(void) {
    Instruction i;

    char line[] = "mneg w11, w12, w13";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_MSUB);
    assert(i.data.multiply.rd == 11);
    assert(i.data.multiply.rn == 12);
    assert(i.data.multiply.rm == 13);
    assert(i.data.multiply.ra == 31);
    assert(i.data.multiply.sf == false);

    printf("Test parse mneg alias: OK\n");
}

void test_parse_b_forward(void) {
    Instruction i;
    char line[] = "b my_loop";
    uint64 current_pc = 0x10;

    SymbolTable *table = create_symbol_table();
    symbol_table_add(table, "my_loop", 0x24);

    bool routed = parse_line(line, &i, table, current_pc);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_UNCONDITIONAL_BRANCH);

    assert(i.data.uncond_branch.simm26 == 5);

    printf("Test parse b forward: OK\n");

    free_symbol_table(table);
}

void test_parse_b_backward(void) {
    Instruction i;
    char line[] = "b start_func";
    uint64 current_pc = 0x100;

    SymbolTable *table = create_symbol_table();
    symbol_table_add(table, "start_func", 0x40);

    bool routed = parse_line(line, &i, table, current_pc);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_UNCONDITIONAL_BRANCH);

    assert(i.data.uncond_branch.simm26 == -48);

    printf("Test parse b backward: OK\n");
    free_symbol_table(table);
}

void test_parse_br_register(void) {
    Instruction i;
    char line[] = "br x8";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_BR);
    assert(i.data.reg_branch.xn == 8);

    printf("Test parse br register: OK\n");
}

void test_parse_b_cond_forward(void) {
    Instruction i;
    char line[] = "b.eq exit_label";
    uint64 current_pc = 0x8;

    SymbolTable *table = create_symbol_table();
    symbol_table_add(table, "exit_label", 0x20);

    bool routed = parse_line(line, &i, table, current_pc);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_EQ);

    assert(i.data.cond_branch.simm19 == 6);

    printf("Test parse b.eq forward: OK\n");
    free_symbol_table(table);
}

void test_parse_memory_unsigned(void) {
    Instruction i;
    char line[] = "ldr x0, [x1, #16]";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_SINGLE_DATA_TRANSFER);
    assert(i.data.single_data_transfer.L == true);
    assert(i.data.single_data_transfer.sf == true);
    assert(i.data.single_data_transfer.mode == ADDR_UNSIGNED_OFFSET);
    assert(i.data.single_data_transfer.rt == 0);
    assert(i.data.single_data_transfer.xn == 1);

    assert(i.data.single_data_transfer.offset == 2);

    printf("Test parse memory unsigned: OK\n");
}

void test_parse_memory_pre_indexed(void) {
    Instruction i;
    char line[] = "str w2, [x3, #-4]!";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_SINGLE_DATA_TRANSFER);
    assert(i.data.single_data_transfer.L == false);
    assert(i.data.single_data_transfer.sf == false);
    assert(i.data.single_data_transfer.mode == ADDR_PRE_INDEXED);
    assert(i.data.single_data_transfer.rt == 2);
    assert(i.data.single_data_transfer.xn == 3);

    assert(i.data.single_data_transfer.offset == -4);

    printf("Test parse memory pre-indexed: OK\n");
}

void test_parse_memory_post_indexed(void) {
    Instruction i;
    char line[] = "ldr x4, [x5], #8";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_SINGLE_DATA_TRANSFER);
    assert(i.data.single_data_transfer.L == true);
    assert(i.data.single_data_transfer.mode == ADDR_POST_INDEXED);
    assert(i.data.single_data_transfer.rt == 4);
    assert(i.data.single_data_transfer.xn == 5);
    assert(i.data.single_data_transfer.offset == 8);

    printf("Test parse memory post-indexed: OK\n");
}

void test_parse_memory_register(void) {
    Instruction i;
    char line[] = "str x6, [x7, x8]";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_SINGLE_DATA_TRANSFER);
    assert(i.data.single_data_transfer.L == false);
    assert(i.data.single_data_transfer.mode == ADDR_REGISTER_OFFSET);
    assert(i.data.single_data_transfer.rt == 6);
    assert(i.data.single_data_transfer.xn == 7);
    assert(i.data.single_data_transfer.xm == 8);

    printf("Test parse memory register: OK\n");
}

void test_parse_memory_literal(void) {
    Instruction i;
    char line[] = "ldr x9, target_data";
    uint64_t current_pc = 0x10;

    SymbolTable *table = create_symbol_table();
    symbol_table_add(table, "target_data", 0x30);

    bool routed = parse_line(line, &i, table, current_pc);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_LOAD_LITERAL);
    assert(i.data.load_literal.sf == true);
    assert(i.data.load_literal.rt == 9);

    printf("\nDEBUG --> current_pc: %ld, target_pc: %ld, simm19: %d\n",
       current_pc,
       symbol_table_lookup(table, "target_data"),
       i.data.load_literal.simm19);

    assert(i.data.load_literal.simm19 == 8);

    printf("Test parse memory literal: OK\n");
    free_symbol_table(table);
}

void test_parse_wide_move_no_shift(void) {
    Instruction i;
    char line[] = "movz w5, #0x1234";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_MOVZ);
    assert(i.data.wide_move.sf == false);
    assert(i.data.wide_move.rd == 5);
    assert(i.data.wide_move.imm16 == 0x1234);
    assert(i.data.wide_move.hw == 0);

    printf("Test parse wide move no shift: OK\n");
}

void test_parse_wide_move_with_shift(void) {
    Instruction i;
    char line[] = "movk x7, #0xABCD, lsl #32";

    bool routed = parse_line(line, &i, NULL, 0);

    assert(routed == true);
    assert(i.op_type == OP_TYPE_MOVK);
    assert(i.data.wide_move.sf == true);
    assert(i.data.wide_move.rd == 7);
    assert(i.data.wide_move.imm16 == 0xABCD);
    assert(i.data.wide_move.hw == 2);

    printf("Test parse wide move with shift: OK\n");
}

int main(void) {
    printf("Assembler Arithmetic Parsing Tests\n");
    printf("----------------------------------\n");

    test_parse_line_ignores_labels_and_blanks();

    test_parse_add_immediate_standard();
    test_parse_sub_immediate_shifted();
    test_parse_adds_register_standard();
    test_parse_subs_register_shifted();

    test_parse_cmp_immediate();
    test_parse_cmn_register_shifted();
    test_parse_neg_register();
    test_parse_negs_register_shifted();

    test_parse_and_register_standard();
    test_parse_orr_register_shifted();
    test_parse_eor_register_shifted();
    test_parse_bics_register_standard();

    test_parse_tst_register();
    test_parse_mvn_register_shifted();
    test_parse_mov_register();

    test_parse_madd_standard();
    test_parse_msub_standard();
    test_parse_mul_alias();
    test_parse_mneg_alias();

    test_parse_b_forward();
    test_parse_b_backward();
    test_parse_br_register();
    test_parse_b_cond_forward();

    test_parse_memory_unsigned();
    test_parse_memory_pre_indexed();
    test_parse_memory_register();
    test_parse_memory_literal();

    test_parse_wide_move_no_shift();
    test_parse_wide_move_with_shift();

    printf("----------------------------------\n");
    printf("All Parser tests passed successfully\n");
    return EXIT_SUCCESS;
}
