#include "common/instruction.h"
#include "emulator/decode/decode.h"
#include "emulator/execute/execute.h"
#include "emulator/state/state.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

// Instruction builder helper for arithmetic & logical
Instruction create_arithmetic_inst(OpType op, bool sf, Register rd, Register rn,
                                   Register rm, ShiftType shift, int operand) {
    Instruction i = {.op_type = op};
    i.register_arithmetic_logic.sf = sf;
    i.register_arithmetic_logic.rd = rd;
    i.register_arithmetic_logic.rn = rn;
    i.register_arithmetic_logic.rm = rm;
    i.register_arithmetic_logic.shift = shift;
    i.register_arithmetic_logic.operand = operand;
    return i;
}

// Instruction builder helper for multiplication
Instruction create_multiply_inst(OpType op, bool sf, Register rd, Register rn,
                                 Register rm, Register ra) {
    Instruction i = {.op_type = op};
    i.multiply.sf = sf;
    i.multiply.rd = rd;
    i.multiply.rn = rn;
    i.multiply.rm = rm;
    i.multiply.ra = ra;
    return i;
}

void test_standard_add() {
    State state = {0};

    // Set initial values: X1 = 15, X2 = 25
    write_reg_64(&state, R1, 15);
    write_reg_64(&state, R2, 25);

    // Build: ADD X0, X1, X2 (sf=true, shift=LSL, amount=0)
    Instruction i =
        create_arithmetic_inst(OP_TYPE_REG_ADD, true, R0, R1, R2, SHIFT_LSL, 0);

    // Execute via main dispatcher
    execute_instruction(&state, &i);

    // Verify X0 is 40
    assert(read_reg_64(&state, R0) == 40);
    printf("test_standard_add: OK\n");
}

void test_edge_case_32bit_asr() {
    State state = {0};

    // Set initial values: W1 = 10, W2 = -4 (as a 32-bit unsigned hex)
    write_reg_32(&state, R1, 10);
    write_reg_32(&state, R2, 0xFFFFFFFC); // -4 in 32-bit Two's Complement

    // Build: SUB W0, W1, W2, ASR #1 (sf=false, shift=ASR, amount=1)
    Instruction i = create_arithmetic_inst(OP_TYPE_REG_SUB, false, R0, R1, R2,
                                           SHIFT_ASR, 1);

    execute_instruction(&state, &i);

    // Verify
    // op2 = (-4 >> 1) = -2.
    // val_n - op2 = 10 - (-2) = 12.
    assert(read_reg_32(&state, R0) == 12);

    // Ensure the top 32 bits of the 64-bit register are zero
    assert(read_reg_64(&state, R0) == 12);
    printf("test_edge_case_32bit_asr: OK\n");
}

void test_flags_subs_borrow() {
    State state = {0};

    // Set initial values: X1 = 5, X2 = 10
    write_reg_64(&state, R1, 5);
    write_reg_64(&state, R2, 10);

    // Build: SUBS X0, X1, X2 (sf=true, shift=LSL, amount=0)
    Instruction i = create_arithmetic_inst(OP_TYPE_REG_SUBS, true, R0, R1, R2,
                                           SHIFT_LSL, 0);

    execute_instruction(&state, &i);

    // Verify Math: 5 - 10 = -5
    assert(read_reg_64(&state, R0) == (uint64)-5);

    // Verify Flags
    assert(read_pstate_flag(&state, N) == true);
    assert(read_pstate_flag(&state, Z) == false);
    assert(read_pstate_flag(&state, C) == false);
    assert(read_pstate_flag(&state, V) == false);

    printf("test_flags_subs_borrow: OK\n");
}

void test_flags_unsigned_overflow() {
    State state = {0};

    // Set initial values: X1 = Max uint64, X2 = 5
    write_reg_64(&state, R1, 0xFFFFFFFFFFFFFFFFULL);
    write_reg_64(&state, R2, 5);

    // Build: ADDS X0, X1, X2
    Instruction i = create_arithmetic_inst(OP_TYPE_REG_ADDS, true, R0, R1, R2,
                                           SHIFT_LSL, 0);

    execute_instruction(&state, &i);

    // Verify Math: It should wrap around completely. Max + 5 = 4.
    assert(read_reg_64(&state, R0) == 4);

    // Verify Flags
    assert(read_pstate_flag(&state, C) == true);
    assert(read_pstate_flag(&state, V) == false);

    printf("test_flags_unsigned_overflow: OK\n");
}

void test_flags_signed_overflow_add() {
    State state = {0};

    // Set initial values: X1 = Max int64, X2 = 1
    write_reg_64(&state, R1, 0x7FFFFFFFFFFFFFFFULL);
    write_reg_64(&state, R2, 1);

    // Build: ADDS X0, X1, X2
    Instruction i = create_arithmetic_inst(OP_TYPE_REG_ADDS, true, R0, R1, R2,
                                           SHIFT_LSL, 0);

    execute_instruction(&state, &i);

    // Verify Math: It spilled into the sign bit, becoming exactly Min int64
    assert(read_reg_64(&state, R0) == 0x8000000000000000ULL);

    // Verify Flags
    assert(read_pstate_flag(&state, V) == true);
    assert(read_pstate_flag(&state, N) == true);
    assert(read_pstate_flag(&state, C) == false);

    printf("test_flags_signed_overflow_add: OK\n");
}

void test_flags_signed_underflow_sub() {
    State state = {0};

    // Set initial values: X1 = Min int64 (Most negative number), X2 = 1
    write_reg_64(&state, R1, 0x8000000000000000ULL);
    write_reg_64(&state, R2, 1);

    // Build: SUBS X0, X1, X2
    Instruction i = create_arithmetic_inst(OP_TYPE_REG_SUBS, true, R0, R1, R2,
                                           SHIFT_LSL, 0);

    execute_instruction(&state, &i);

    // Verify Math: It underflowed into Max int64
    assert(read_reg_64(&state, R0) == 0x7FFFFFFFFFFFFFFFULL);

    // Verify Flags
    assert(read_pstate_flag(&state, V) == true);
    assert(read_pstate_flag(&state, N) == false);
    assert(read_pstate_flag(&state, C) == true);

    printf("test_flags_signed_underflow_sub: OK\n");
}

void test_logical_orr() {
    State state = {0};

    // X1 = 10101010, X2 = 01010101
    write_reg_64(&state, R1, 0xAAAAAAAAAAAAAAAAULL);
    write_reg_64(&state, R2, 0x5555555555555555ULL);

    // Build: ORR X0, X1, X2
    Instruction i =
        create_arithmetic_inst(OP_TYPE_ORR, true, R0, R1, R2, SHIFT_LSL, 0);

    execute_instruction(&state, &i);

    // Verify: 1010 | 0101 = 1111 (All 1s)
    assert(read_reg_64(&state, R0) == 0xFFFFFFFFFFFFFFFFULL);
    printf("test_logical_orr: OK\n");
}

void test_logical_bic_32bit() {
    State state = {0};

    // W1 = All 1s, W2 = Bottom half 1s, Top half 0s
    write_reg_32(&state, R1, 0xFFFFFFFF);
    write_reg_32(&state, R2, 0x0000FFFF);

    // Build: BIC W0, W1, W2 (Bitwise Clear: W1 AND NOT W2)
    Instruction i =
        create_arithmetic_inst(OP_TYPE_BIC, false, R0, R1, R2, SHIFT_LSL, 0);

    execute_instruction(&state, &i);

    // Verify: W2 inverted is 0xFFFF0000.
    // 0xFFFFFFFF & 0xFFFF0000 = 0xFFFF0000.
    assert(read_reg_32(&state, R0) == 0xFFFF0000);

    // CRITICAL: Ensure the top 32 bits of the 64-bit register are completely
    // zero!
    assert(read_reg_64(&state, R0) == 0x00000000FFFF0000ULL);
    printf("test_logical_bic_32bit: OK\n");
}

void test_logical_ands_flags() {
    State state = {0};

    // Set both to just the 64-bit sign bit
    write_reg_64(&state, R1, 0x8000000000000000ULL);
    write_reg_64(&state, R2, 0x8000000000000000ULL);

    // Build: ANDS X0, X1, X2
    Instruction i =
        create_arithmetic_inst(OP_TYPE_ANDS, true, R0, R1, R2, SHIFT_LSL, 0);

    execute_instruction(&state, &i);

    // Verify Math
    assert(read_reg_64(&state, R0) == 0x8000000000000000ULL);

    // Verify Flags
    assert(read_pstate_flag(&state, N) == true);  // Sign bit is 1
    assert(read_pstate_flag(&state, Z) == false); // Result is not zero
    assert(read_pstate_flag(&state, C) == false); // Always 0 for logic
    assert(read_pstate_flag(&state, V) == false); // Always 0 for logic

    printf("test_logical_ands_flags: OK\n");
}

void test_multiply_madd() {
    State state = {0};

    // Set initial values: X1 (rn) = 5, X2 (rm) = 6, X3 (ra) = 10
    write_reg_64(&state, R1, 5);
    write_reg_64(&state, R2, 6);
    write_reg_64(&state, R3, 10);

    // Build: MADD X0, X1, X2, X3 (sf=true)
    // Formula: X0 = X3 + (X1 * X2)
    Instruction i = create_multiply_inst(OP_TYPE_MADD, true, R0, R1, R2, R3);

    execute_instruction(&state, &i);

    // Verify: 10 + (5 * 6) = 40
    assert(read_reg_64(&state, R0) == 40);
    printf("test_multiply_madd: OK\n");
}

void test_multiply_msub_32bit() {
    State state = {0};

    // Set initial values: W1 (rn) = 3, W2 (rm) = 4, W3 (ra) = 20
    write_reg_32(&state, R1, 3);
    write_reg_32(&state, R2, 4);
    write_reg_32(&state, R3, 20);

    // Build: MSUB W0, W1, W2, W3 (sf=false)
    // Formula: W0 = W3 - (W1 * W2)
    Instruction i = create_multiply_inst(OP_TYPE_MSUB, false, R0, R1, R2, R3);

    execute_instruction(&state, &i);

    // Verify: 20 - (3 * 4) = 8
    assert(read_reg_32(&state, R0) == 8);

    // Ensure the top 32 bits of the 64-bit register are zero
    assert(read_reg_64(&state, R0) == 8);

    printf("test_multiply_msub_32bit: OK\n");
}

void test_dpir(void) {
    printf("Data Processing Instruction (Register) Tests\n");
    printf("--------------------------------------------\n");

    // Arithmetic Operation Tests
    test_standard_add();
    test_edge_case_32bit_asr();
    test_flags_subs_borrow();
    test_flags_unsigned_overflow();
    test_flags_signed_overflow_add();
    test_flags_signed_underflow_sub();

    // Logical Operation Tests
    test_logical_orr();
    test_logical_bic_32bit();
    test_logical_ands_flags();

    // Multiply Operation Tests
    test_multiply_madd();
    test_multiply_msub_32bit();

    printf("--------------------------------------------\n");
    printf("All tests passed\n");
}
