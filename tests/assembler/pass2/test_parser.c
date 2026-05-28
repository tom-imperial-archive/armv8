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
    assert(parse_line(label_line, &i, NULL) == false);

    char blank_line[] = "   \n";
    assert(parse_line(blank_line, &i, NULL) == false);

    printf("Test parse line ignores labels and blanks: OK\n");
}

void test_parse_add_immediate_standard(void) {
    Instruction i;
    char line[] = "add x0, x1, #42";

    bool routed = parse_line(line, &i, NULL);

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

    bool routed = parse_line(line, &i, NULL);

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

    bool routed = parse_line(line, &i, NULL);

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

    bool routed = parse_line(line, &i, NULL);

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

    bool routed = parse_line(line, &i, NULL);

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

    bool routed = parse_line(line, &i, NULL);

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

    bool routed = parse_line(line, &i, NULL);

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

    bool routed = parse_line(line, &i, NULL);

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

    bool routed = parse_line(line, &i, NULL);

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

    bool routed = parse_line(line, &i, NULL);

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

    bool routed = parse_line(line, &i, NULL);

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

    bool routed = parse_line(line, &i, NULL);

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

    bool routed = parse_line(line, &i, NULL);

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

    bool routed = parse_line(line, &i, NULL);

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

    bool routed = parse_line(line, &i, NULL);

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

    printf("----------------------------------\n");
    printf("All Parser tests passed successfully\n");
    return EXIT_SUCCESS;
}
