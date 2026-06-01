#include <stdlib.h>
#include <stdio.h>
#include "assembler/pass1/scanner.h"
#include "assembler/symbol_table/symbol_table.h"
#include "assembler/pass2/parser.h"
#include "assembler/encode/encode.h"
#include "common/error.h"
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
        error(REQUIRE_CORRECT_ARGS, NULL);
    }

    char *in = argv[1];
    char *out = argv[2];
    if (out == NULL)
    {
        out = "out.bin";
    }

    FILE *file_in = fopen(in, "rb");
    if (file_in == NULL)
    {
        error(ERROR_READING_FILE, str_error_info(in));
    }

    char buf[MAX_FILE_LINE_LENGTH];
    SymbolTable *table = create_symbol_table();
    uint64 output_size = scan_file(in, table);
    uint32 *res = malloc(output_size);

    if (res == NULL)
    {
        error(FAILED_TO_ALLOCATE, NULL);
    }
    // todo graceful error handling if over line length
    // todo no hard upper limit on liness
    int64 PC = 0;
    int instr_index = 0;
    while (fgets(buf, MAX_FILE_LINE_LENGTH, file_in) != NULL)
    {
        Instruction instr;
        bool instruction = parse_line(buf, &instr, table, PC);
        if (instruction)
        {
            res[instr_index] = encode_instruction(&instr);
            printf("Instruction %d: %08x\n", instr_index, res[instr_index]);
            PC += 4;
            instr_index++;
        }
    }
    fclose(file_in);

    // Write results to file
    FILE *file_out = fopen(out, "wb");
    if (file_out == NULL)
    {
        error(ERROR_WRITING_FILE, str_error_info(out));
    }
    else
    {
        fwrite(res, sizeof(uint32), instr_index, file_out);
        fclose(file_out);
    }

    free(res);
    free_symbol_table(table);
    return EXIT_SUCCESS;
}
