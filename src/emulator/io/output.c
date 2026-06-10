#include "common/error.h"
#include "emulator/io/filehandlers.h"
#include "emulator/state/state.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fwrite_all(State *state, char *path) {
    FILE *f = fopen(path, "w");

    if (f == NULL) {
        ERROR((Error){.type = ERROR_WRITING_FILE, .str = path});
    }

    fprint_all_registers(state, f);
    fprint_nonzero_memory(state, f);

    fclose(f);
}

void print_all(State *state) {
    // Pass standard output to the fprint functions
    fprint_all_registers(state, stdout);
    fprint_nonzero_memory(state, stdout);
}
