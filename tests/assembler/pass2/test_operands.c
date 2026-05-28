#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "assembler/pass2/operands.h"

void test_parse_register_64_bit(void) {
    bool is_64_bit;

    assert(parse_register("x0", &is_64_bit) == 0);
    assert(is_64_bit == true);

    assert(parse_register("x12", &is_64_bit) == 12);
    assert(is_64_bit == true);

    assert(parse_register("x31", &is_64_bit) == 31);
    assert(is_64_bit == true);

    printf("Test parse register 64-bit: OK\n");
}

void test_parse_register_32_bit(void) {
    bool is_64_bit;

    assert(parse_register("w0", &is_64_bit) == 0);
    assert(is_64_bit == false);

    assert(parse_register("w7", &is_64_bit) == 7);
    assert(is_64_bit == false);

    assert(parse_register("w31", &is_64_bit) == 31);
    assert(is_64_bit == false);

    printf("Test parse register 32-bit: OK\n");
}

void test_parse_register_special_and_boundary(void) {
    bool is_64_bit;

    assert(parse_register("xzr", &is_64_bit) == 31);
    assert(is_64_bit == true);

    assert(parse_register("wzr", &is_64_bit) == 31);
    assert(is_64_bit == false);

    assert(parse_register("sp", &is_64_bit) == 31);
    assert(is_64_bit == true);

    assert(parse_register("x1", &is_64_bit) == 1);
    assert(parse_register("x30", &is_64_bit) == 30);

    printf("Test parse register special and boundary: OK\n");
}

void test_parse_immediate_decimal(void) {
    assert(parse_immediate("#0") == 0);
    assert(parse_immediate("#1") == 1);
    assert(parse_immediate("#5") == 5);
    assert(parse_immediate("#42") == 42);
    assert(parse_immediate("#255") == 255);
    assert(parse_immediate("#1000") == 1000);

    assert(parse_immediate("#-1") == -1);
    assert(parse_immediate("#-12") == -12);
    assert(parse_immediate("#-255") == -255);
    assert(parse_immediate("#-1000") == -1000);

    printf("Test parse immediate decimal: OK\n");
}

void test_parse_immediate_hex(void) {
    assert(parse_immediate("#0x0") == 0x0);
    assert(parse_immediate("#0x1") == 0x1);
    assert(parse_immediate("#0xA") == 0xA);
    assert(parse_immediate("#0x1A") == 0x1A);
    assert(parse_immediate("#0xff") == 0xff);
    assert(parse_immediate("#0xFF") == 0xFF);
    assert(parse_immediate("#0x10") == 0x10);
    assert(parse_immediate("#0x100") == 0x100);

    assert(parse_immediate("#0X1A") == 0x1A);
    assert(parse_immediate("#0XFF") == 0xFF);

    assert(parse_immediate("#-0x1") == -0x1);
    assert(parse_immediate("#-0x1A") == -0x1A);
    assert(parse_immediate("#-0xff") == -0xff);

    printf("Test parse immediate hex: OK\n");
}

void test_parse_immediate_zero_variants(void) {
    assert(parse_immediate("#0") == 0);
    assert(parse_immediate("#0x0") == 0);
    assert(parse_immediate("#-0") == 0);

    printf("Test parse immediate zero variants: OK\n");
}

void test_calculate_offset_forward(void) {
    assert(calculate_offset(0x00, 0x00) == 0);
    assert(calculate_offset(0x00, 0x04) == 1);
    assert(calculate_offset(0x00, 0x08) == 2);
    assert(calculate_offset(0x00, 0x28) == 10);
    assert(calculate_offset(0x1000, 0x2000) == 0x400);

    printf("Test calculate offset forward: OK\n");
}

void test_calculate_offset_backward(void) {
    assert(calculate_offset(0x04, 0x00) == -1);
    assert(calculate_offset(0x08, 0x00) == -2);
    assert(calculate_offset(0x28, 0x00) == -10);
    assert(calculate_offset(0x2000, 0x1000) == -0x400);

    printf("Test calculate offset backward: OK\n");
}

void test_calculate_offset_nonzero_base(void) {
    assert(calculate_offset(0x100, 0x110) == 4);
    assert(calculate_offset(0x110, 0x100) == -4);
    assert(calculate_offset(0x400, 0x800) == 0x100);

    printf("Test calculate offset nonzero base: OK\n");
}

int main(void) {
    printf("Operand Parsing Tests\n");
    printf("---------------------\n");

    test_parse_register_64_bit();
    test_parse_register_32_bit();
    test_parse_register_special_and_boundary();

    test_parse_immediate_decimal();
    test_parse_immediate_hex();
    test_parse_immediate_zero_variants();

    test_calculate_offset_forward();
    test_calculate_offset_backward();
    test_calculate_offset_nonzero_base();

    printf("---------------------\n");
    printf("All Operand tests passed successfully\n");

    return EXIT_SUCCESS;
}
