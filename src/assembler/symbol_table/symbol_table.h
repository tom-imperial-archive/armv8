#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "utils/types.h"

// Individual label-address pairs
typedef struct {
    char *label;
    uint64 address;
} Symbol;

// Dynamic array storing all pairs
typedef struct {
    Symbol *entries;
    int count;
    int capacity;
} SymbolTable;

// Allocates memory and initialises
SymbolTable *create_symbol_table(void);

// Adds new label, and resizes array if needed
// Note we must store the label ourselves,
//     since the pointer passed will change as scanner.c runs
void symbol_table_add(SymbolTable *table, char *label, uint64 address);

// Returns address represented by a label
uint64 symbol_table_lookup(SymbolTable *table, char *label);

// Frees memory from the table, and from storing the labels
void free_symbol_table(SymbolTable *table);

#endif
