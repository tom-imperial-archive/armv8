#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "common/error.h"
#include "emulator/io/filehandlers.h"
#include "emulator/execute/execute.h"
#include <string.h>
#include "emulator/io/output.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        error(REQUIRE_CORRECT_ARGS, NULL);
        return EXIT_FAILURE;
    }

    size_t size;
    uint32 *fileBuffer = readfile(argv[1], &size);
    if (fileBuffer == NULL)
    {
        print_err(ERROR_READING_FILE, file_error_info(argv[1]));
        return EXIT_FAILURE;
    }

    State *state = init_state();
    write(state->m, 0, (uint8 *)fileBuffer, size * sizeof(uint32));
    free(fileBuffer);

    // Main loop
    Instruction *i = malloc(sizeof(Instruction));

    if (i == NULL)
    {
        destroy_state(state);
        print_err(FAILED_TO_ALLOCATE, NULL);
        return EXIT_FAILURE;
    }

    bool shouldHalt = false;

    while (!shouldHalt)
    {
        uint32 instruction = read_mem_32(state, read_reg_64(state, PC));
        // todo make this nicer
        DecodeResult r = decode(instruction, i);
        switch (r)
        {
        case DECODE_SUCCESS:
        {
            shouldHalt = execute_instruction(state, i->op_type, i);
        };
        break;
        case DECODE_UNDEFINED_OPCODE:
        {
            shouldHalt = true;
            print_err(UNDEFINED_OPCODE, instruction_error_info(instruction));
        }
        break;
        }
    }

    char *outputFile = argv[2];
    if (outputFile == NULL)
    {
        outputFile = "emulate.out";
    }
    fwrite_all(state, outputFile);
    print_all(state);

    free(i);
    destroy_state(state);
    return EXIT_SUCCESS;
}
