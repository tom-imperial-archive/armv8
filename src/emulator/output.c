#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "emulator/state/state.h"
#include "emulator/decode/filehandlers.h"
#define REG_OUT_PRINT_BUFFER 1024

void fwrite_all(State *state, const char *path) {
    char registers_out[REG_OUT_PRINT_BUFFER] = {0};
    char *nonzero_out = sprint_nonzero_memory(state);

    sprint_all_registers(state, registers_out);

    assert(writefile(path, registers_out, nonzero_out));

    free(nonzero_out);
}

void print_all(State *state) {
    char registers_out[REG_OUT_PRINT_BUFFER] = {0};
    char *nonzero_out = sprint_nonzero_memory(state);

    sprint_all_registers(state, registers_out);

    printf("%s\n%s\n", registers_out, nonzero_out);

    free(nonzero_out);
}
