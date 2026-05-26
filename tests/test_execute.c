#include "test_execute.h"
#include <stdio.h>
#include "emulator/decode/execute.h"
#include <assert.h>

void test_dpi_imm()
{
    uint64 val1 = 0xFFF;
    State *state = init_state();
    write_reg_64(state, R0, val1);

    Instruction i = {
        .op_type = OP_TYPE_ADD,
        .data.immediate_arithmetic = {
            .sf = true,
            .rd = R1,
            .rn = R0,
            .sh = false,
            .imm12 = val1,
        }};

    execute_instruction(state, OP_TYPE_ADD, &i);

    assert(read_reg_64(state, R1) == 2 * val1);

    uint64 val2 = 0xFFFFFFFFFFFFFFFF;
    write_reg_64(state, R0, val2);

    i.op_type = OP_TYPE_ADDS;
    i.data.immediate_arithmetic.sh = true;
    i.data.immediate_arithmetic.rd = R2;
    i.data.immediate_arithmetic.sf = true;
    execute_instruction(state, OP_TYPE_ADDS, &i);

    assert(read_reg_64(state, R2) == val2 + (val1 << 12));
    assert(read_pstate_flag(state, N) == false);
    assert(read_pstate_flag(state, Z) == false);
    assert(read_pstate_flag(state, C) == true);
    assert(read_pstate_flag(state, V) == false);

    uint32 val3 = 0xFFFFFFFF;
    i.op_type = OP_TYPE_ADDS;
    write_reg_32(state, R0, val3);
    i.data.immediate_arithmetic.sh = false;
    i.data.immediate_arithmetic.sf = false;
    i.data.immediate_arithmetic.rn = R0;
    i.data.immediate_arithmetic.rd = R3;
    i.data.immediate_arithmetic.imm12 = 0;
    execute_instruction(state, OP_TYPE_ADDS, &i);
    assert(read_pstate_flag(state, N) == true);

    uint32 val4 = 0x7FFFFFFF;
    write_reg_64(state, R0, val2);
    i.op_type = OP_TYPE_ADDS;
    i.data.immediate_arithmetic.rn = R0;
    i.data.immediate_arithmetic.imm12 = val4;
    execute_instruction(state, OP_TYPE_ADDS, &i);
    assert(read_pstate_flag(state, N) == false);
    assert(read_pstate_flag(state, Z) == false);
    assert(read_pstate_flag(state, C) == true);
    assert(read_pstate_flag(state, V) == false);

    write_reg_64(state, R0, val2);
    i.op_type = OP_TYPE_SUB;
    i.data.immediate_arithmetic.imm12 = val1;
    i.data.immediate_arithmetic.sf = 1;
    execute_instruction(state, OP_TYPE_SUB, &i);
    assert(read_reg_64(state, R3) == val2 - val1);

    i.op_type = OP_TYPE_SUBS;
    execute_instruction(state, OP_TYPE_SUBS, &i);
    assert(read_reg_64(state, R3) == val2 - val1);
    assert(read_pstate_flag(state, N) == true);
    assert(read_pstate_flag(state, Z) == false);
    assert(read_pstate_flag(state, C) == true);
    assert(read_pstate_flag(state, V) == false);
    printf("Test add adds sub subs: OK\n");

    write_reg_64(state, R0, 0);
    Instruction i1 = {
        .op_type = OP_TYPE_MOVN,
        .data.wide_move = {
            .sf = false,
            .hw = 1,
            .imm16 = 0x1234,
            .rd = R0}};

    execute_instruction(state, OP_TYPE_MOVN, &i1);
    assert(read_reg_32(state, R0) == 0xEDCBFFFF);
    i1.data.wide_move.sf = true;
    execute_instruction(state, OP_TYPE_MOVN, &i1);
    assert(read_reg_64(state, R0) == 0xFFFFFFFFEDCBFFFF);

    i1.op_type = OP_TYPE_MOVZ;
    execute_instruction(state, OP_TYPE_MOVZ, &i1);
    assert(read_reg_64(state, R0) == i1.data.wide_move.imm16);
    i1.data.wide_move.sf = false;
    execute_instruction(state, OP_TYPE_MOVZ, &i1);
    assert(read_reg_32(state, R0) == i1.data.wide_move.imm16);

    i1.op_type = OP_TYPE_MOVK;
    i1.data.wide_move.imm16 = 0xABCD;
    i1.data.wide_move.hw = 0;

    write_reg_32(state, R0, 0x12345678UL);
    execute_instruction(state, OP_TYPE_MOVK, &i1);
    assert(read_reg_32(state, R0) == 0x1234ABCDU);
    write_reg_32(state, R0, 0x12345678UL);
    i1.data.wide_move.sf = true;
    i1.data.wide_move.hw = 2;
    execute_instruction(state, OP_TYPE_MOVK, &i1);
    assert(read_reg_64(state, R0) == 0xABCD12345678UL);


    printf("Test wide move (movn, movz, movk): OK\n");
}

void test_execute(void)
{
    printf("Execute Tests\n");
    printf("-------------\n");

    test_dpi_imm();

    printf("-------------\n");
    printf("All tests passed\n");
}
