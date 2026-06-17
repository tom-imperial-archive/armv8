#include "assembler/encode/encode.h"
#include "assembler/pass1/scanner.h"
#include "assembler/pass2/parser.h"
#include "assembler/symbol_table/symbol_table.h"
#include "common/error.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_FILE_LINE_LENGTH 120

int main(int argc, char **argv) {
    /*
    MAIN LOOP RUNS AS FOLLOWS
    1. First pass - use scanner.c to populate SymbolTable
    2. Second pass - one line at a time:
        call parse_line,
        then call encode_instruction,
        then write the encoded instruction to the output file
    */

    if (argc < 2) {
        ERROR((Error){.type = REQUIRE_CORRECT_ARGS});
    }

    char *in = argv[1];
    char *out = argv[2];

    if (out == NULL) {
        out = "out.bin";
    }

    FILE *file_in = fopen(in, "rb");
    if (file_in == NULL) {
        ERROR((Error){.type = ERROR_READING_FILE, .str = in});
    }

    char buf[MAX_FILE_LINE_LENGTH];
    SymbolTable *table = create_symbol_table();
    // todo handle file logic
    scan_file(file_in, table);

    // Open the output file
    FILE *file_out = fopen(out, "wb");
    if (file_out == NULL) {
        fclose(file_in);
        free_symbol_table(table);
        ERROR((Error){.type = ERROR_WRITING_FILE, .str = out});
    }

    Instruction instruction;
    uint64 pc = 0;

    // Main assembler loop
    // For each instruction in the input, we parse and then encode
    // The result of the encoding is then written to the output file
    while (fgets(buf, MAX_FILE_LINE_LENGTH, file_in) != NULL) {
        if (parse_line(buf, &instruction, table, pc)) {
            uint32 bits = encode_instruction(&instruction);
            fwrite(&bits, sizeof(uint32), 1, file_out);
            pc += 4;
        }
    }

    fclose(file_in);
    fclose(file_out);

    free_symbol_table(table);
    return EXIT_SUCCESS;
}
