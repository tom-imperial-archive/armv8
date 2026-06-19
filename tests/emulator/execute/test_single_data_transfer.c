#include "test_single_data_transfer.h"
#include "common/instruction.h"
#include "emulator/execute/execute.h"
#include "emulator/state/state.h"
#include <assert.h>
#include <stdbool.h>

int test_single_data_transfer(void) {
    State *state;
    const int RT = 3;
    const int XN = 5;
    const uint64 BASE_ADDRESS = 0x2000;
    const uint64 VAL_64 = 0xDEADC0DECAFEBABAULL;

    // Pre-Indexed Load 64-bit (LDR Xt, [Xn, #16]!)
    // Expected: Address = Base + 16. Writeback Base = Base + 16.
    Instruction instr_pre = {
        .op_type = OP_TYPE_SINGLE_DATA_TRANSFER,
        .single_data_transfer = {.rt = RT,
                                 .xn = XN,
                                 .L = true,
                                 .sf = true,
                                 .mode = ADDR_PRE_INDEXED,
                                 .offset = 16},
    };

    state = init_state();
    write_reg_64(state, XN, BASE_ADDRESS);
    write_mem_64(state, BASE_ADDRESS + 16, VAL_64);

    execute_single_data_transfer(state, &instr_pre);

    assert(read_reg_64(state, RT) == VAL_64);
    assert(read_reg_64(state, XN) == BASE_ADDRESS + 16); // Writeback check
    destroy_state(state);

    // Post-Indexed Store 64-bit (STR Xt, [Xn], #24)
    // Expected: Address = Base. Writeback Base = Base + 24.
    Instruction instr_post = {
        .op_type = OP_TYPE_SINGLE_DATA_TRANSFER,
        .single_data_transfer = {.rt = RT,
                                 .xn = XN,
                                 .L = false,
                                 .sf = true,
                                 .mode = ADDR_POST_INDEXED,
                                 .offset = 24},
    };

    state = init_state();
    write_reg_64(state, XN, BASE_ADDRESS);
    write_reg_64(state, RT, VAL_64);
    write_mem_64(state, BASE_ADDRESS, 0x0ULL); // Clear memory at initial base

    execute_single_data_transfer(state, &instr_post);

    assert(read_mem_64(state, BASE_ADDRESS) ==
           VAL_64); // Stored at ORIGINAL address
    assert(read_reg_64(state, XN) ==
           BASE_ADDRESS + 24); // Xn is updated AFTER transfer
    destroy_state(state);

    // Pre-Indexed Negative Offset (LDR Xt, [Xn, #-8]!)
    // Expected: Address = Base - 8. Writeback Base = Base - 8.
    Instruction instr_pre_neg = {
        .op_type = OP_TYPE_SINGLE_DATA_TRANSFER,
        .single_data_transfer = {.rt = RT,
                                 .xn = XN,
                                 .L = true,
                                 .sf = true,
                                 .mode = ADDR_PRE_INDEXED,
                                 .offset = -8},
    };

    state = init_state();
    write_reg_64(state, XN, BASE_ADDRESS);
    write_mem_64(state, BASE_ADDRESS - 8, VAL_64);

    execute_single_data_transfer(state, &instr_pre_neg);

    assert(read_reg_64(state, RT) == VAL_64);
    assert(read_reg_64(state, XN) == BASE_ADDRESS - 8);
    destroy_state(state);

    return 0;
}
