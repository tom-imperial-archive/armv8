#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "assembler/pass2/operands.h"

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name, expr) do { \
    tests_run++; \
    if (expr) { tests_passed++; printf("PASS: %s\n", name); } \
    else { printf("FAIL: %s\n", name); } \
} while(0)

static void test_parse_reg(void) {
    bool is_64_bit;

    // --- 64-bit x registers ---
    is_64_bit = false;
    TEST("x0 returns 0",    parse_register("x0",  &is_64_bit) == 0);
    TEST("x0 sets 64-bit",  is_64_bit == true);

    is_64_bit = false;
    TEST("x12 returns 12",  parse_register("x12", &is_64_bit) == 12);
    TEST("x12 sets 64-bit", is_64_bit == true);

    is_64_bit = false;
    TEST("x31 returns 31",  parse_register("x31", &is_64_bit) == 31);
    TEST("x31 sets 64-bit", is_64_bit == true);

    // --- 32-bit w registers ---
    is_64_bit = true;
    TEST("w0 returns 0",     parse_register("w0",  &is_64_bit) == 0);
    TEST("w0 clears 64-bit", is_64_bit == false);

    is_64_bit = true;
    TEST("w7 returns 7",     parse_register("w7",  &is_64_bit) == 7);
    TEST("w7 clears 64-bit", is_64_bit == false);

    is_64_bit = true;
    TEST("w31 returns 31",   parse_register("w31", &is_64_bit) == 31);
    TEST("w31 clears 64-bit",is_64_bit == false);

    // --- zero register aliases ---
    is_64_bit = false;
    TEST("xzr returns 31",   parse_register("xzr", &is_64_bit) == 31);
    TEST("xzr sets 64-bit",  is_64_bit == true);

    is_64_bit = true;
    TEST("wzr returns 31",   parse_register("wzr", &is_64_bit) == 31);
    TEST("wzr clears 64-bit",is_64_bit == false);

    // --- boundary register numbers ---
    is_64_bit = false;
    TEST("x1 returns 1",    parse_register("x1",  &is_64_bit) == 1);
    TEST("x30 returns 30",  parse_register("x30", &is_64_bit) == 30);
}

static void test_parse_imm(void) {
    // --- positive decimal ---
    TEST("decimal #0",       parse_immediate("#0")   == 0);
    TEST("decimal #1",       parse_immediate("#1")   == 1);
    TEST("decimal #5",       parse_immediate("#5")   == 5);
    TEST("decimal #42",      parse_immediate("#42")  == 42);
    TEST("decimal #255",     parse_immediate("#255") == 255);
    TEST("decimal #1000",    parse_immediate("#1000") == 1000);

    // --- negative decimal ---
    TEST("negative #-1",     parse_immediate("#-1")   == -1);
    TEST("negative #-12",    parse_immediate("#-12")  == -12);
    TEST("negative #-255",   parse_immediate("#-255") == -255);
    TEST("negative #-1000",  parse_immediate("#-1000") == -1000);

    // --- positive hex (lowercase x) ---
    TEST("hex #0x0",         parse_immediate("#0x0")   == 0x0);
    TEST("hex #0x1",         parse_immediate("#0x1")   == 0x1);
    TEST("hex #0xA",         parse_immediate("#0xA")   == 0xA);
    TEST("hex #0x1A",        parse_immediate("#0x1A")  == 0x1A);
    TEST("hex #0xff",        parse_immediate("#0xff")  == 0xff);
    TEST("hex #0xFF",        parse_immediate("#0xFF")  == 0xFF);
    TEST("hex #0x10",        parse_immediate("#0x10")  == 0x10);
    TEST("hex #0x100",       parse_immediate("#0x100") == 0x100);

    // --- positive hex (uppercase X) ---
    TEST("hex #0X1A",        parse_immediate("#0X1A")  == 0x1A);
    TEST("hex #0XFF",        parse_immediate("#0XFF")  == 0xFF);

    // --- negative hex ---
    TEST("negative hex #-0x1",   parse_immediate("#-0x1")  == -0x1);
    TEST("negative hex #-0x1A",  parse_immediate("#-0x1A") == -0x1A);
    TEST("negative hex #-0xff",  parse_immediate("#-0xff") == -0xff);

    // --- zero variants ---
    TEST("decimal zero #0",  parse_immediate("#0")    == 0);
    TEST("hex zero #0x0",    parse_immediate("#0x0")  == 0);
    TEST("neg zero #-0",     parse_immediate("#-0")   == 0);
}

int main(void) {
    printf("=== parse_register ===\n");
    test_parse_reg();

    printf("\n=== parse_immediate ===\n");
    test_parse_imm();

    printf("\n%d/%d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
