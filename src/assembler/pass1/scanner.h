#ifndef SCANNER_H
#define SCANNER_H

#include "assembler/symbol_table/symbol_table.h"
#include <stdio.h>

extern void scan_file(FILE *file, SymbolTable *table);

#endif
