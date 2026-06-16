#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include "assembler/symbol_table/symbol_table.h"

extern void scan_file(FILE *file, SymbolTable *table);

#endif
