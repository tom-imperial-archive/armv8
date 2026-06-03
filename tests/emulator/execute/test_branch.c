#include "emulator/decode/decode.h"
#include "emulator/state/state.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

// Forward declaration of the main execute function
bool execute_instruction(State *state, OpType op, Instruction *i);

// Instruction builder helpers
Instruction create_uncond_branch_inst(int32_t simm26) {
    Instruction i = {.op_type = OP_TYPE_AL};
    i.uncond_branch.simm26 = simm26;
    return i;
}

Instruction create_cond_branch_inst(OpType op, int32_t simm19) {
    Instruction i = {.op_type = op};
    i.cond_branch.simm19 = simm19;
    return i;
}

Instruction create_reg_branch_inst(Register xn) {
    Instruction i = {.op_type = OP_TYPE_BR};
    i.reg_branch.xn = xn;
    return i;
}

void test_unconditional_branch() {
    State state = {0};
    state.PC = 0x1000; // Arbitrary starting address

    // Build: b #5 (Branch forward by 5 instructions)
    // Offset should be 5 * 4 = 20
    Instruction i = create_uncond_branch_inst(5);

    execute_instruction(&state, i.op_type, &i);

    // Verify PC advanced by 20 bytes
    assert(state.PC == 0x1014); // 0x1000 + 20(decimal) = 0x1014
    printf("test_unconditional_branch: OK\n");
}

void test_register_branch() {
    State state = {0};
    state.PC = 0x1000;

    // Set X5 to contain our target address
    write_reg_64(&state, R5, 0x8048000);

    // Build: br X5
    Instruction i = create_reg_branch_inst(R5);

    execute_instruction(&state, i.op_type, &i);

    // Verify PC jumped directly to the address inside X5
    assert(state.PC == 0x8048000);
    printf("test_register_branch: OK\n");
}

void test_conditional_branch_taken() {
    State state = {0};
    state.PC = 0x1000;

    // Setup: We want to test BEQ (Branch if Equal). This requires Z flag to
    // be 1.
    write_pstate_flag(&state, Z, true);

    // Build: b.eq #-3 (Branch backwards by 3 instructions)
    // Offset should be -3 * 4 = -12
    Instruction i = create_cond_branch_inst(OP_TYPE_EQ, -3);

    execute_instruction(&state, i.op_type, &i);

    // Verify PC went backwards by 12 bytes
    assert(state.PC == 0x1000 - 12);
    printf("test_conditional_branch_taken: OK\n");
}

void test_conditional_branch_not_taken() {
    State state = {0};
    state.PC = 0x1000;

    // Setup: We test BNE (Branch if Not Equal). This requires Z flag to be 0.
    // However, we will set Z to 1, meaning the condition is FALSE.
    write_pstate_flag(&state, Z, true);

    // Build: b.ne #10
    Instruction i = create_cond_branch_inst(OP_TYPE_NE, 10);

    execute_instruction(&state, i.op_type, &i);

    // Verify PC did NOT change because the condition failed, only incremented
    assert(state.PC == 0x1000 + 0x4);
    printf("test_conditional_branch_not_taken: OK\n");
}

void test_conditional_branch_lt_taken() {
    State state = {0};
    state.PC = 0x1000;

    // Setup: BLT (Branch Less Than) requires N != V.
    write_pstate_flag(&state, N, true);
    write_pstate_flag(&state, V, false);

    // Build: b.lt #2
    Instruction i = create_cond_branch_inst(OP_TYPE_LT, 2);

    execute_instruction(&state, i.op_type, &i);

    // Verify PC advanced by 8 bytes
    assert(state.PC == 0x1008);
    printf("test_conditional_branch_lt_taken: OK\n");
}

void test_branch(void) {
    printf("Branch Instruction Tests\n");
    printf("--------------------------------------------\n");

    test_unconditional_branch();
    test_register_branch();
    test_conditional_branch_taken();
    test_conditional_branch_not_taken();
    test_conditional_branch_lt_taken();

    printf("--------------------------------------------\n");
    printf("All branch tests passed\n");
}
