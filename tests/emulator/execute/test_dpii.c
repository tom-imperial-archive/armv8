#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "emulator/decode/execute.h"
#include "common/hashset.h"

// Instruction Builder Helpers
Instruction create_imm_arithmetic_inst(OpType op, bool sf, Register rd, Register rn, uint16_t imm12, bool sh) {
    Instruction i = { .op_type = op };
    i.data.immediate_arithmetic.sf = sf;
    i.data.immediate_arithmetic.rd = rd;
    i.data.immediate_arithmetic.rn = rn;
    i.data.immediate_arithmetic.imm12 = imm12;
    i.data.immediate_arithmetic.sh = sh;
    return i;
}

Instruction create_wide_move_inst(OpType op, bool sf, Register rd, uint16_t imm16, uint8_t hw) {
    Instruction i = { .op_type = op };
    i.data.wide_move.sf = sf;
    i.data.wide_move.rd = rd;
    i.data.wide_move.imm16 = imm16;
    i.data.wide_move.hw = hw;
    return i;
}

void test_add_imm_standard() {
    State *state = init_state();
    uint64 val1 = 0xFFF;
    write_reg_64(state, R0, val1);

    Instruction i = create_imm_arithmetic_inst(OP_TYPE_ADD, true, R1, R0, val1, false);
    execute_instruction(state, i.op_type, &i);

    assert(read_reg_64(state, R1) == 2 * val1);

    printf("test_add_imm_standard: OK\n");
    destroy_state(state);
}

void test_adds_imm_flags_shifted() {
    State *state = init_state();
    uint64 val1 = 0xFFF;
    uint64 val2 = 0xFFFFFFFFFFFFFFFFULL;
    write_reg_64(state, R0, val2);

    Instruction i = create_imm_arithmetic_inst(OP_TYPE_ADDS, true, R2, R0, val1, true);
    execute_instruction(state, i.op_type, &i);

    assert(read_reg_64(state, R2) == val2 + (val1 << 12));
    assert(read_pstate_flag(state, N) == false);
    assert(read_pstate_flag(state, Z) == false);
    assert(read_pstate_flag(state, C) == true);
    assert(read_pstate_flag(state, V) == false);

    printf("test_adds_imm_flags_shifted: OK\n");
    destroy_state(state);
}

void test_subs_imm_standard() {
    State *state = init_state();
    uint64 val1 = 0xFFF;
    uint64 val2 = 0xFFFFFFFFFFFFFFFFULL;
    write_reg_64(state, R0, val2);

    Instruction i = create_imm_arithmetic_inst(OP_TYPE_SUBS, true, R3, R0, val1, false);
    execute_instruction(state, i.op_type, &i);

    assert(read_reg_64(state, R3) == val2 - val1);
    assert(read_pstate_flag(state, N) == true);
    assert(read_pstate_flag(state, Z) == false);
    assert(read_pstate_flag(state, C) == true);
    assert(read_pstate_flag(state, V) == false);

    printf("test_subs_imm_standard: OK\n");
    destroy_state(state);
}

void test_wide_move_movk() {
    State *state = init_state();
    write_reg_32(state, R0, 0x12345678UL);

    Instruction i = create_wide_move_inst(OP_TYPE_MOVK, false, R0, 0xABCD, 0);
    execute_instruction(state, i.op_type, &i);
    assert(read_reg_32(state, R0) == 0x1234ABCDU);

    write_reg_64(state, R0, 0x12345678UL);
    Instruction i2 = create_wide_move_inst(OP_TYPE_MOVK, true, R0, 0xABCD, 2);
    execute_instruction(state, i2.op_type, &i2);
    assert(read_reg_64(state, R0) == 0xABCD12345678ULL);

    printf("test_wide_move_movk: OK\n");
    destroy_state(state);
}

void test_subs_imm_carry_flag_no_borrow() {
    State *state = init_state();
    write_reg_64(state, R0, 5);

    Instruction i = create_imm_arithmetic_inst(OP_TYPE_SUBS, true, R1, R0, 0, false);
    execute_instruction(state, i.op_type, &i);

    assert(read_pstate_flag(state, C) == true);

    printf("test_subs_imm_carry_flag_no_borrow: OK\n");
    destroy_state(state);
}

void test_subs_imm_overflow_flag_underflow() {
    State *state = init_state();
    write_reg_64(state, R0, 0x8000000000000000ULL);

    // SUBS R1, R0, #1
    Instruction i = create_imm_arithmetic_inst(OP_TYPE_SUBS, true, R1, R0, 1, false);
    execute_instruction(state, i.op_type, &i);

    assert(read_pstate_flag(state, V) == true);

    printf("test_subs_imm_overflow_flag_underflow: OK\n");
    destroy_state(state);
}

void test_movz_applies_shift_correctly() {
    State *state = init_state();

    Instruction i = create_wide_move_inst(OP_TYPE_MOVZ, true, R0, 0xABCD, 1);
    execute_instruction(state, i.op_type, &i);

    assert(read_reg_64(state, R0) == 0xABCD0000ULL);

    printf("test_movz_applies_shift_correctly: OK\n");
    destroy_state(state);
}

void test_movn_64bit_shift_prevents_truncation() {
    State *state = init_state();

    Instruction i = create_wide_move_inst(OP_TYPE_MOVN, true, R0, 0xFFFF, 3);
    execute_instruction(state, i.op_type, &i);

    assert(read_reg_64(state, R0) == 0x0000FFFFFFFFFFFFULL);

    printf("test_movn_64bit_shift_prevents_truncation: OK\n");
    destroy_state(state);
}

void test_dpii(void) {
    printf("Data Processing Immediate Tests\n");
    printf("--------------------------------\n");

    test_add_imm_standard();
    test_adds_imm_flags_shifted();
    test_subs_imm_standard();
    test_wide_move_movk();
    test_subs_imm_carry_flag_no_borrow();
    test_subs_imm_overflow_flag_underflow();
    test_movz_applies_shift_correctly();
    test_movn_64bit_shift_prevents_truncation();

    printf("--------------------------------\n");
    printf("All DPI Immediate tests passed\n");
}
