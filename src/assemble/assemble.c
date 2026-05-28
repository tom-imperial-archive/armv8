#include <stdlib.h>
#include <stdio.h>
#include "assembler/pass1/scanner.h"
#include "assembler/symbol_table/symbol_table.h"
#include "assembler/pass2/parser.h"
#include "assembler/encode/encode.h"
#define MAX_FILE_LINE_LENGTH 120

int main(int argc, char **argv)
{
    /*
    MAIN LOOP RUNS AS FOLLOWS
    1. First pass - use scanner.c to populate SymbolTable
    2. Second pass - one line at a time:
        call parse_line,
        then call encode_instruction,
        then write the result to the output file
    */

    if (argc < 2)
    {
        fprintf(stderr, "Require file in: <file_in> [file_out]\n");
        return EXIT_FAILURE;
    }

    char *in = argv[1];
    char *out = argv[2];
    if (out == NULL) {
        //todo check for a memory leak here
        out = "out.bin";
    }

    SymbolTable *table = create_symbol_table();
    uint64 output_size = scan_file(in, table);

    FILE *file_in = fopen(in, "rb");
    if (file_in == NULL)
    {
        printf("Failed to open file %s\n", in);
        exit(EXIT_FAILURE);
    }

    char buf[MAX_FILE_LINE_LENGTH];

    uint32 *res = malloc(output_size);
    //todo graceful error handling if over line length
    //todo no hard upper limit on lines
    for (int i = 0; i < output_size / sizeof(uint32); i++){
        fgets(buf, MAX_FILE_LINE_LENGTH, file_in);
        Instruction instr;
        parse_line(buf, &instr, table);
        *(res + i) = encode_instruction(&instr);
        printf("Instruction %d: %08x\n", i, res[i]);
    }
    fclose(file_in);

    // Write results to file
    FILE *file_out = fopen(out, "wb");
    if (file_out == NULL) {
        printf("Failed to open file %s\n", out);
    }

    fwrite(res, output_size / sizeof(uint32), sizeof(uint32), file_out);
    fclose(file_out);
    return EXIT_SUCCESS;
}
