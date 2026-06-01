#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "emulator/state/state.h"
#include "emulator/io/filehandlers.h"
#include "common/error.h"

void fwrite_all(State *state, char *path) {
    char registers_out[REG_PRINT_BUFFER_SIZE] = {0};
    char *nonzero_out = sprint_nonzero_memory(state);

    sprint_all_registers(state, registers_out);

    if (!writefile(path, registers_out, nonzero_out)) {
        print_err(ERROR_WRITING_FILE, str_error_info(path));
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
