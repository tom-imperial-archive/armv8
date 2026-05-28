#include "assembler/symbol_table/symbol_table.h"

/*
THIS IS PASS 1 OF THE TWO PASS APPROACH

We read through each line, tracking the current memory address (making sure to ignore comments).
When we enccounter a label, we add it, and the current address, to the symbol table.
*/

// This should read through the file, and populate a SymbolTable mapping labels to addresses
// Note this is def not the best way to pass the file around, but it's sufficient for this sketch.
void scan_file(char *filename, SymbolTable *table) {
    return;
}
