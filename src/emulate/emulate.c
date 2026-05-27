#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "emulator/decode/filehandlers.h"
#include "emulator/decode/execute.h"
#include <string.h>
#include "emulator/output.h"

int main(int argc, char **argv)
{
    State *state = init_state();
    if (argc < 2)
    {
        fprintf(stderr, "Require file in: <file_in> [file_out]\n");
        return EXIT_FAILURE;
    }

    size_t size;
    uint32 *fileBuffer = readfile(argv[1], &size);
    if (fileBuffer == NULL)
    {
        printf("Null pointer for file\n");
        return EXIT_FAILURE;
    }


    write(state->m, 0, (uint8 *) fileBuffer, size * sizeof(uint32));

    // Main loop
    bool shouldHalt = false;

    Instruction *i = malloc(sizeof(Instruction));

    if (i == NULL) {
        printf("Not enough memory\n");
        return EXIT_FAILURE;
    }

    while (!shouldHalt)
    {
        uint32 instruction = read_mem_32(state, read_reg_64(state, PC));
        //todo make this nicer
        DecodeResult r = decode(instruction, i);

        switch (r)
        {
        case DECODE_SUCCESS:
        {
            shouldHalt = execute_instruction(state, i->op_type, i);
            // DEBUGGG
            // print_all(state);
        };
        break;
        case DECODE_UNDEFINED_OPCODE:
            printf("Undefined OPCODE in instruction %x\n", instruction);
            shouldHalt = true;
            break;
        }
    }

    char* outputFile = argv[2];
    if (outputFile == NULL) {
        outputFile = "emulate.out";
    }
    fwrite_all(state, outputFile);
    print_all(state);

    free(i);

    destroy_state(state);
    return EXIT_SUCCESS;
}
