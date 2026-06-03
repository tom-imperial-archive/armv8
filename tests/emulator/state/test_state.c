#include "emulator/state/state.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void test_create(void) {
    State *state = init_state();
    for (int i = 0; i <= R30; i++) {
        assert(read_reg_64(state, i) == 0);
        assert(read_reg_32(state, i) == 0);
    }

    assert(read_reg_64(state, ZR) == 0);
    assert(read_reg_64(state, PC) == 0);
    assert(read_reg_64(state, SP) == 0);
    assert(read_reg_32(state, ZR) == 0);
    assert(read_reg_32(state, SP) == 0);

    assert(read_pstate_flag(state, N) == 0);
    assert(read_pstate_flag(state, Z) == 1);
    assert(read_pstate_flag(state, C) == 0);
    assert(read_pstate_flag(state, V) == 0);
    printf("Test create: OK\n");
}

void test_write_reg(void) {
    const uint64 UINT64_MAX_VALUE = 0xFFFFFFFFFFFFFFFFULL;
    const uint32 UINT32_MAX_VALUE = 0xFFFFFFFFUL;

    State *state = init_state();

    write_reg_64(state, R5, UINT64_MAX_VALUE);
    assert(read_reg_64(state, R5) == UINT64_MAX_VALUE);
    assert(read_reg_32(state, R5) == 0xFFFFFFFFUL);

    write_reg_32(state, R5, UINT32_MAX_VALUE);
    // Test that write_reg_32 correctly clears the upper 32 bits
    assert(read_reg_64(state, R5) >> 32 == 0);
    assert(read_reg_32(state, R5) == UINT32_MAX_VALUE);
    printf("Test write (32 and 64 bit): OK\n");
}

int main(void) {
    printf("State Tests\n");
    printf("-------------\n");
    test_create();
    test_write_reg();
    printf("-------------\n");
    printf("All tests passed\n");
    return 0;
}
