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

int main(void) {
    printf("Assembler Arithmetic Parsing Tests\n");
    printf("----------------------------------\n");

    test_parse_line_ignores_labels_and_blanks();

    test_parse_add_immediate_standard();
    test_parse_sub_immediate_shifted();

    test_parse_adds_register_standard();
    test_parse_subs_register_shifted();

    printf("----------------------------------\n");
    printf("All Parser tests passed successfully\n");
    return EXIT_SUCCESS;
}
