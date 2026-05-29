#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

#define INITIAL_CAPACITY 10

/*
This abstract data structure will act as a dictionary from labels to addresses.
It will be populated by scanner.c, and then used by parser.c.
Since there will be a very low number of labels, it suffices to implement this
    using an array, similarly to [(Label, Address)] in Haskell.
*/

// PLACEHOLDERS TO BE IMPLEMENTED

// Allocates memory and initialises
SymbolTable* create_symbol_table(void) {
    SymbolTable *table = malloc(sizeof(SymbolTable));
    Symbol *entries = malloc(INITIAL_CAPACITY * sizeof(Symbol));
    table->entries = entries;
    table->count = 0;
    table->capacity = INITIAL_CAPACITY;
    return table;
}

// Adds new label, and resizes array if needed
// Note we must store the label ourselves,
//     since the pointer passed will change as scanner.c runs
void symbol_table_add(SymbolTable *table, char *label, uint64 address) {
    // Table is full so reallocate
    if (table->count == table->capacity) {
        table->capacity *= 2;
        table->entries = realloc(table->entries, table->capacity * sizeof(Symbol));
    }
    table->entries[table->count].label = strdup(label);
    table->entries[table->count].address = address;
    table->count++;
}

// Returns address represented by a label
uint64 symbol_table_lookup(SymbolTable *table, char *label) {
    for (int i = 0; i < table->count; i++) {
        if (!strcmp(table->entries[i].label, label)) {
            return table->entries[i].address;
        }
    }
    // FAILURE - should never happen but we should handle somehow
    exit(EXIT_FAILURE);
}

// Frees memory from the table, and from storing the labels
void free_symbol_table(SymbolTable *table) {
    for (int i = 0; i < table->count; i++) {
        free(table->entries[i].label);
    }
    free(table->entries);
    free(table);
}
