#include "common/error.h"
#include "emulator/execute/execute.h"
#include "emulator/io/filehandlers.h"
#include "emulator/io/output.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        ERROR((Error){.type = REQUIRE_CORRECT_ARGS});
        return EXIT_FAILURE;
    }

    size_t size;
    uint32 *fileBuffer = readfile(argv[1], &size);
    if (fileBuffer == NULL) {
        ERROR((Error){.type = ERROR_READING_FILE, .str = argv[1]});
        return EXIT_FAILURE;
    }

    State *state = init_state();
    write(state->m, 0, (uint8 *)fileBuffer, size * sizeof(uint32));
    free(fileBuffer);

    bool shouldHalt = false;
    Instruction i;
    while (!shouldHalt) {
        uint32 instruction = read_mem_32(state, read_reg_64(state, PC));
        decode(instruction, &i);
        shouldHalt = execute_instruction(state, &i);
    }

    char *outputFile = argv[2];
    if (outputFile == NULL) {
        outputFile = "emulate.out";
    }
    fwrite_all(state, outputFile);
    print_all(state);

    destroy_state(state);
    return EXIT_SUCCESS;
}
