#include "common/error.h"
#include "emulator/io/filehandlers.h"
#include "emulator/state/state.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fwrite_all(State *state, char *path) {
    char registers_out[REG_PRINT_BUFFER_SIZE] = {0};
    char *nonzero_out = sprint_nonzero_memory(state);

    sprint_all_registers(state, registers_out);

    if (!writefile(path, registers_out, nonzero_out)) {
        ERROR((Error){.type = ERROR_WRITING_FILE, .str = path});
    }

    free(nonzero_out);
}

void print_all(State *state) {
    char registers_out[REG_PRINT_BUFFER_SIZE] = {0};
    char *nonzero_out = sprint_nonzero_memory(state);

    sprint_all_registers(state, registers_out);

    printf("%s\n%s\n", registers_out, nonzero_out);

    free(nonzero_out);
}
