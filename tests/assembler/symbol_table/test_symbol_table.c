#include "assembler/symbol_table/symbol_table.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_create(void) {
    SymbolTable *table = create_symbol_table();

    assert(table != NULL);
    assert(table->count == 0);
    assert(table->capacity == 10);

    free_symbol_table(table);
    printf("Test create: OK\n");
}

void test_add_and_lookup(void) {
    SymbolTable *table = create_symbol_table();

    // Add a standard label
    symbol_table_add(table, "my_loop", 0x04);
    symbol_table_add(table, "end_loop", 0x1C);

    // Assert that looking them up returns the exact addresses we just added
    assert(symbol_table_lookup(table, "my_loop") == 0x04);
    assert(symbol_table_lookup(table, "end_loop") == 0x1C);

    free_symbol_table(table);
    printf("Test add and lookup: OK\n");
}

void test_reallocation(void) {
    SymbolTable *table = create_symbol_table();

    // The table starts with a capacity of 10.
    // Adding 15 items forces it to trigger the realloc() block.
    for (int i = 0; i < 15; i++) {
        symbol_table_add(table, "dummy_label", i * 4);
    }

    // Assert that all 15 items were successfully tracked
    assert(table->count == 15);

    // Assert that the capacity successfully doubled from 10 to 20
    assert(table->capacity == 20);

    free_symbol_table(table);
    printf("Test reallocation OK\n");
}

void test_memory_ownership(void) {
    SymbolTable *table = create_symbol_table();

    // Create a temporary stack buffer just like scanner.c will use
    char temp_buffer[50];
    strcpy(temp_buffer, "first_label");

    // Add it to the table
    symbol_table_add(table, temp_buffer, 0x10);

    // Completely wipe and overwrite the temporary buffer
    strcpy(temp_buffer, "garbage_text");

    // Assert that the Symbol Table still remembers "first_label"
    assert(symbol_table_lookup(table, "first_label") == 0x10);

    free_symbol_table(table);
    printf("Test memory ownership: OK\n");
}

int main(void) {
    printf("Symbol Table Tests\n");
    printf("------------------\n");

    test_create();
    test_add_and_lookup();
    test_reallocation();
    test_memory_ownership();

    printf("------------------\n");
    printf("All Symbol Table tests passed\n");
    return EXIT_SUCCESS;
}
