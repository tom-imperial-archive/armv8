#include "test_execute.h"
#include <stdio.h>
#include "emulator/decode/execute.h"
#include <assert.h>

void test_dpi_imm() {
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
        }
    };

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
    printf("Actual: %x %d\n", read_reg_32(state, R3), read_reg_32(state, R3) >> 31);
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

    /*
    Tests to run:
    sub bit shift
    subs w/wo
    */
}

void test_execute(void)
{
    printf("Execute Tests\n");
    printf("-------------\n");

    test_dpi_imm();

    printf("-------------\n");
    printf("All tests passed\n");
}
