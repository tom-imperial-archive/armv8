#ifndef SCANNER_H
#define SCANNER_H

#include "assembler/symbol_table/symbol_table.h"

// This should read through the file, and populate a SymbolTable mapping labels to addresses
// Note this is def not the best way to pass the file around, but it's sufficient for this sketch.
void scan_file(char *filename, SymbolTable *table);

#endif
