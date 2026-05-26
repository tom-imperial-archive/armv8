// #include "test_single_data_transfer.h"

// #include "emulator/decode.h"
// #include "emulator/decode/execute.h"

// #include <assert.h>
// #include <stdbool.h>

// int test_single_data_transfer(void) {
//     State* state;
//     const int RT = 3;
//     const int XN = 5;
//     const uint64 BASE_ADDRESS = 0x2000;

//     const uint64 VAL_64 = 0xDEADC0DECAFEBABAULL;
//     const uint32 VAL_32 = 0xCAFEBABA;

//     // TEST 1: Load 64-bit Unsigned Offset (LDR Xt, [Xn, #offset])
//     // Specification: Address = Xn + (imm12 * 8). No register writeback.
//     int imm12_scaled_64 = 4; // Represents an offset of 4 * 8 = 32 bytes
//     uint64 target_addr_1 = BASE_ADDRESS + (imm12_scaled_64 * 8);

//     Instruction instr_load_64 = {
//         .op_type = OP_TYPE_SINGLE_DATA_TRANSFER,
//         .data.single_data_transfer = {
//             .rt = RT,
//             .xn = XN,
//             .L = true,                  // Load operation
//             .sf = true,                 // 64-bit transfer size
//             .U = true,                  // Unsigned Offset Mode
//             .offset = imm12_scaled_64,  // Stored unscaled in instruction
//         },
//     };

//     state = init_state();
//     write_reg_64(state, XN, BASE_ADDRESS);
//     write_mem_64(state, target_addr_1, VAL_64);

//     execute_single_data_transfer(state, &instr_load_64);

//     assert(read_reg_64(state, RT) == VAL_64);
//     assert(read_reg_64(state, XN) == BASE_ADDRESS); // Base register must remain unchanged
//     destroy_state(state);


//     // TEST 2: Load 32-bit Unsigned Offset (LDR Wt, [Xn, #offset])
//     // Specification: Address = Xn + (imm12 * 4). Top 32-bits of Xt are zeroed.
//     int imm12_scaled_32 = 6; // Represents an offset of 6 * 4 = 24 bytes
//     uint64 target_addr_2 = BASE_ADDRESS + (imm12_scaled_32 * 4);

//     Instruction instr_load_32 = {
//         .op_type = OP_TYPE_SINGLE_DATA_TRANSFER,
//         .data.single_data_transfer = {
//             .rt = RT,
//             .xn = XN,
//             .L = true,
//             .sf = false,                // 32-bit transfer size
//             .U = true,
//             .offset = imm12_scaled_32,
//         },
//     };

//     state = init_state();
//     write_reg_64(state, XN, BASE_ADDRESS);
//     write_mem_64(state, target_addr_2, VAL_64); // Memory holds a full 64-bit pattern

//     execute_single_data_transfer(state, &instr_load_32);

//     // Architecture rules: 32-bit loads clear the upper 32 bits of the general register
//     assert(read_reg_64(state, RT) == (uint64)VAL_32);
//     destroy_state(state);


//     // TEST 3: Pre-Indexed Load 64-bit (LDR Xt, [Xn, #simm9]!)
//     // Specification: Address = Xn + simm9. Base register updates with new address.
//     int simm9_offset = 99;
//     uint64 target_addr_3 = BASE_ADDRESS + simm9_offset;

//     Instruction instr_pre_index = {
//         .op_type = OP_TYPE_SINGLE_DATA_TRANSFER,
//         .data.single_data_transfer = {
//             .rt = RT,
//             .xn = XN,
//             .L = true,
//             .sf = true,
//             .U = false,                 // U = 0 indicates indexed or unscaled mode
//             .offset = simm9_offset,     // Negative signed 9-bit immediate
//         },
//     };

//     state = init_state();
//     write_reg_64(state, XN, BASE_ADDRESS);
//     write_mem_64(state, target_addr_3, VAL_64);

//     execute_single_data_transfer(state, &instr_pre_index);

//     assert(read_reg_64(state, RT) == VAL_64);
//     assert(read_reg_64(state, XN) == target_addr_3); // Write-back rule: Xn updates to target address
//     destroy_state(state);


//     // TEST 4: Post-Indexed Store 64-bit (STR Xt, [Xn], #simm9)
//     // Specification: Address = Xn. Afterwards, Xn = Xn + simm9.
//     // int post_simm9_offset = 97;
//     // uint64 expected_final_xn = BASE_ADDRESS + post_simm9_offset;

//     // Instruction instr_post_index = {
//     //     .op_type = OP_TYPE_SINGLE_DATA_TRANSFER,
//     //     .data.single_data_transfer = {
//     //         .rt = RT,
//     //         .xn = XN,
//     //         .L = false,                 // Store operation
//     //         .sf = true,
//     //         .U = false,
//     //         .offset = post_simm9_offset,
//     //     },
//     // };

//     // state = init_state();
//     // write_reg_64(state, XN, BASE_ADDRESS);
//     // write_reg_64(state, RT, VAL_64);
//     // write_mem_64(state, BASE_ADDRESS, 0x0ULL); // Clear target block

//     // execute_single_data_transfer(state, &instr_post_index);

//     // assert(read_mem_64(state, BASE_ADDRESS) == VAL_64); // Stored immediately at base address
//     // assert(read_reg_64(state, XN) == expected_final_xn); // Post-index rule: Xn updates after transfer
//     // destroy_state(state);

//     return 0;
// }
#include "test_single_data_transfer.h"

#include "emulator/decode.h"
// #include "emulator/state/state.h"
#include "emulator/decode/execute.h"

#include <assert.h>
#include <stdbool.h>

// -----------------------------------------------------------------------------
// MACRO: Perfectly reconstructs the 12-bit offset field extracted by your decode.c
// -> decode.c extracts: (input & 0x003FFC00) >> 10
// -> Bits 10 to 2 of offset: simm9 (masked to 9 bits)
// -> Bit 1 of offset: I (1 = Pre-indexed, 0 = Post-indexed)
// -> Bit 0 of offset: Fixed 1
// -----------------------------------------------------------------------------
#define BUILD_INDEX_OFFSET(simm9, I) ((((simm9) & 0x1FF) << 2) | (((I) & 1) << 1) | 1)

int test_single_data_transfer(void) {
    State* state;
    const int RT = 3;
    const int XN = 5;
    const uint64 BASE_ADDRESS = 0x2000;
    const uint64 VAL_64 = 0xDEADC0DECAFEBABAULL;

    // =========================================================================
    // TEST 1: Pre-Indexed Load 64-bit (LDR Xt, [Xn, #16]!)
    // Expected: Address = Base + 16. Writeback Base = Base + 16.
    // =========================================================================
    Instruction instr_pre = {
        .op_type = OP_TYPE_SINGLE_DATA_TRANSFER,
        .data.single_data_transfer = {
            .rt = RT, .xn = XN, .L = true, .sf = true, .U = false,
            .offset = BUILD_INDEX_OFFSET(16, 1) // simm9 = 16, I = 1
        },
    };

    state = init_state();
    write_reg_64(state, XN, BASE_ADDRESS);
    write_mem_64(state, BASE_ADDRESS + 16, VAL_64);

    execute_single_data_transfer(state, &instr_pre);

    assert(read_reg_64(state, RT) == VAL_64);
    assert(read_reg_64(state, XN) == BASE_ADDRESS + 16); // Writeback check
    destroy_state(state);

    // =========================================================================
    // TEST 2: Post-Indexed Store 64-bit (STR Xt, [Xn], #24)
    // Expected: Address = Base. Writeback Base = Base + 24.
    // =========================================================================
    Instruction instr_post = {
        .op_type = OP_TYPE_SINGLE_DATA_TRANSFER,
        .data.single_data_transfer = {
            .rt = RT, .xn = XN, .L = false, .sf = true, .U = false,
            .offset = BUILD_INDEX_OFFSET(24, 0) // simm9 = 24, I = 0
        },
    };

    state = init_state();
    write_reg_64(state, XN, BASE_ADDRESS);
    write_reg_64(state, RT, VAL_64);
    write_mem_64(state, BASE_ADDRESS, 0x0ULL); // Clear memory at initial base

    execute_single_data_transfer(state, &instr_post);

    assert(read_mem_64(state, BASE_ADDRESS) == VAL_64); // Stored at ORIGINAL address
    assert(read_reg_64(state, XN) == BASE_ADDRESS + 24); // Xn is updated AFTER transfer
    destroy_state(state);

    // =========================================================================
    // TEST 3: Pre-Indexed Negative Offset (LDR Xt, [Xn, #-8]!)
    // Expected: Address = Base - 8. Writeback Base = Base - 8.
    // NOTE: If this specific test fails, your emulator has a sign-extension bug.
    // =========================================================================
    Instruction instr_pre_neg = {
        .op_type = OP_TYPE_SINGLE_DATA_TRANSFER,
        .data.single_data_transfer = {
            .rt = RT, .xn = XN, .L = true, .sf = true, .U = false,
            .offset = BUILD_INDEX_OFFSET(-8, 1) // simm9 = -8, I = 1
        },
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
