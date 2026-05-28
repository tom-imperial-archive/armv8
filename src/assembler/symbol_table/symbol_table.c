#include <stdlib.h>
#include "symbol_table.h"

/*
This abstract data structure will act as a dictionary from labels to addresses.
It will be populated by scanner.c, and then used by parser.c.
Since there will be a very low number of labels, it suffices to implement this
    using an array, similarly to [(Label, Address)] in Haskell.
*/

// PLACEHOLDERS TO BE IMPLEMENTED

// Allocates memory and initialises
SymbolTable* create_symbol_table(void) {
    return NULL;
}

// Adds new label, and resizes array if needed
// Note we must store the label ourselves,
//     since the pointer passed will change as scanner.c runs
void symbol_table_add(SymbolTable *table, char *label, uint64 address) {
    return;
}

// Returns address represented by a label
uint64 symbol_table_lookup(SymbolTable *table, char *label) {
    return 0;
}

// Frees memory from the table, and from storing the labels
void free_symbol_table(SymbolTable *table) {
    return;
}
