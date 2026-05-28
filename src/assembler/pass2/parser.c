#include "common/instruction.h"
#include "assembler/symbol_table/symbol_table.h"
#include <stdbool.h>

/*
THIS IS PASS 2 OF THE TWO PASS APPROACH

Here, we tokenise each line in the file, and build it into a struct
We are reusing the same structs as we did in deocde (now stored in common/instruction.h)
Note operands.c provides helpers that we should use here.

We should use function pointers here (as suggested in the spec) to avoid a very messy if/then/else structure.
*/

// Takes a single line of assembly
// If it's an instruction, we populate the instruction struct and return true.
// If the line is blank, a comment, or a label, we simply return false.
bool parse_line(char *line, Instruction *i, SymbolTable *table) {
    return 0;
}
