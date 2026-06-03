#include "test_load_literal.h"

#include "emulator/decode/decode.h"
#include "emulator/execute/execute.h"

#include <assert.h>

int test_load_literal(void) {
    State *state;
    uint64 expected_address;
    const int RT = 2;
    const int RT2 = 6;
    const int SIMM19_POSITIVE = 7;
    const int SIMM19_NEGATIVE = -347;
    const int SIMM19_MAX = 0x3FFFF;
    const int SIMM19_MIN = -0x40000;

    // Check value of rt is set to the whatever value is at address PC + simm19
    // * 4.

    // Checking with sf = false
    Instruction instruction1 = {
        .op_type = OP_TYPE_LOAD_LITERAL,
        .data.load_literal =
            {
                .rt = RT,
                .sf = false,
                .simm19 = SIMM19_POSITIVE,
            },
    };
    const uint64 WRITTEN_VALUE = 0x00ADEFBABACADABA;
    const uint32 EXPECTED_VALUE = 0xBACADABA;
    state = init_state();
    expected_address = state->PC + SIMM19_POSITIVE * 4;
    write_mem_64(state, expected_address, WRITTEN_VALUE);
    execute_load_literal(state, &instruction1);
    assert(read_reg_64(state, RT) == EXPECTED_VALUE);
    assert(read_mem_64(state, expected_address) == WRITTEN_VALUE);

    // Checking with sf = true
    Instruction instruction2 = {
        .op_type = OP_TYPE_LOAD_LITERAL,
        .data.load_literal =
            {
                .rt = RT,
                .sf = true,
                .simm19 = SIMM19_POSITIVE,
            },
    };
    destroy_state(state);
    state = init_state();
    expected_address = state->PC + SIMM19_POSITIVE * 4;
    write_mem_64(state, expected_address, EXPECTED_VALUE);
    execute_load_literal(state, &instruction2);
    assert(read_reg_64(state, RT) == EXPECTED_VALUE);
    assert(read_mem_64(state, expected_address) == EXPECTED_VALUE);

    // Checking with negative simm19
    Instruction instruction3 = {
        .op_type = OP_TYPE_LOAD_LITERAL,
        .data.load_literal =
            {
                .rt = RT,
                .sf = true,
                .simm19 = SIMM19_NEGATIVE,
            },
    };
    destroy_state(state);
    state = init_state();
    state->PC = -4 * SIMM19_NEGATIVE + 1;
    expected_address = state->PC + SIMM19_NEGATIVE * 4;
    write_mem_64(state, expected_address, EXPECTED_VALUE);
    execute_load_literal(state, &instruction3);
    assert(read_reg_64(state, RT) == EXPECTED_VALUE);
    assert(read_mem_64(state, expected_address) == EXPECTED_VALUE);

    // Checking with postiive simm19
    Instruction instruction4 = {
        .op_type = OP_TYPE_LOAD_LITERAL,
        .data.load_literal =
            {
                .rt = RT,
                .sf = true,
                .simm19 = SIMM19_POSITIVE,
            },
    };
    destroy_state(state);
    state = init_state();
    expected_address = state->PC + SIMM19_POSITIVE * 4;
    write_mem_64(state, expected_address, EXPECTED_VALUE);
    execute_load_literal(state, &instruction4);
    assert(read_reg_64(state, RT) == EXPECTED_VALUE);
    assert(read_mem_64(state, expected_address) == EXPECTED_VALUE);

    // Checking with zero simm19
    Instruction instruction5 = {
        .op_type = OP_TYPE_LOAD_LITERAL,
        .data.load_literal =
            {
                .rt = RT,
                .sf = true,
                .simm19 = SIMM19_POSITIVE,
            },
    };
    destroy_state(state);
    state = init_state();
    expected_address = state->PC + SIMM19_POSITIVE * 4;
    write_mem_64(state, expected_address, EXPECTED_VALUE);
    execute_load_literal(state, &instruction5);
    assert(read_reg_64(state, RT) == EXPECTED_VALUE);
    assert(read_mem_64(state, expected_address) == EXPECTED_VALUE);

    // Checking with max simm19
    Instruction instruction6 = {
        .op_type = OP_TYPE_LOAD_LITERAL,
        .data.load_literal =
            {
                .rt = RT,
                .sf = true,
                .simm19 = SIMM19_MAX,
            },
    };
    destroy_state(state);
    state = init_state();
    expected_address = state->PC + SIMM19_MAX * 4;
    write_mem_64(state, expected_address, EXPECTED_VALUE);
    execute_load_literal(state, &instruction6);
    assert(read_reg_64(state, RT) == EXPECTED_VALUE);
    assert(read_mem_64(state, expected_address) == EXPECTED_VALUE);

    // Checking with min simm19
    Instruction instruction7 = {
        .op_type = OP_TYPE_LOAD_LITERAL,
        .data.load_literal =
            {
                .rt = RT,
                .sf = true,
                .simm19 = SIMM19_MIN,
            },
    };
    destroy_state(state);
    state = init_state();
    state->PC = -4 * SIMM19_MIN + 1;
    expected_address = state->PC + SIMM19_MIN * 4;
    write_mem_64(state, expected_address, EXPECTED_VALUE);
    execute_load_literal(state, &instruction7);
    assert(read_reg_64(state, RT) == EXPECTED_VALUE);
    assert(read_mem_64(state, expected_address) == EXPECTED_VALUE);

    // Checking with different value of RT
    Instruction instruction8 = {
        .op_type = OP_TYPE_LOAD_LITERAL,
        .data.load_literal =
            {
                .rt = RT2,
                .sf = true,
                .simm19 = SIMM19_POSITIVE,
            },
    };
    destroy_state(state);
    state = init_state();
    expected_address = state->PC + SIMM19_POSITIVE * 4;
    write_mem_64(state, expected_address, EXPECTED_VALUE);
    execute_load_literal(state, &instruction8);
    assert(read_reg_64(state, RT2) == EXPECTED_VALUE);
    assert(read_mem_64(state, expected_address) == EXPECTED_VALUE);

    return 0;
}
