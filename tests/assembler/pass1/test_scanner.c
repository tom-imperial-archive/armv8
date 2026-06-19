#include "assembler/pass1/scanner.h"
#include <assert.h>
#include <stdio.h>

void test_scanfile(void) {
    SymbolTable *table = create_symbol_table();
    FILE *out = fopen("tests/assembler/pass1/test_scanner.txt", "r");
    assert(out != NULL);
    scan_file(out, table);
    assert(symbol_table_lookup(table, ".label1") == 0);
    assert(symbol_table_lookup(table, "label2") == 12);
    assert(symbol_table_lookup(table, "_label$") == 20);
    fclose(out);

    symbol_table_lookup(table, "_label$");
    free_symbol_table(table);
    printf("Scanner Test: OK\n");
}

int main(void) {
    printf("Scanner Tests\n");
    printf("-------------\n");
    test_scanfile();
    printf("-------------\n");
    printf("All tests passed\n");
    return 0;
}
