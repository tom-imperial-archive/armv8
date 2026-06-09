#include "assembler/symbol_table/symbol_table.h"
#include "common/error.h"
#include "utils/types.h"
#include <assert.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define is_label_char(c) isalnum(c) || c == '_' || c == '.' || c == '\\'

#define INSTRUCTION_SIZE 4
#define MAX_FILE_LINE_LENGTH 120
#define REGEX_FLAGS (REG_EXTENDED | REG_ICASE)
/*
THIS IS PASS 1 OF THE TWO PASS APPROACH

We read through each line, tracking the current memory address (making sure to
ignore comments). When we encounter a label, we add it, and the current
address, to the symbol table.

This accepts any preceeding whitespace
*/

// Regex: [a-zA-Z_\.]([a-zA-Z0-9$_\.])*
static void read_line(char *buf, uint64 *address, SymbolTable *table) {
    // Remove whitespace
    while (isspace(*buf)) buf++;

    if (*buf == '\0') {
        // Line is just whitespace
        return;
    }

    // Line is not whitespace
    int i = 0;
    while(is_label_char(buf[i])) {
        i++;
    }

    if (buf[i] == ':') {
        // We have a label
        // Do not increment
        buf[i] = '\0';
        symbol_table_add(table, buf, *address);
    } else {
        *address += 4;
    }

}

// This reads through the file, and populate a SymbolTable mapping labels
// to addresses Note this is def not the best way to pass the file around, but
// it's sufficient for this sketch.
uint64 scan_file(char *filename, SymbolTable *table) {
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        ERROR((Error){.type = ERROR_READING_FILE, .str = filename});
    }

    char buf[MAX_FILE_LINE_LENGTH];

    uint64 binary_size = 0;
    while (fgets(buf, MAX_FILE_LINE_LENGTH, f)) {
        read_line(buf, &binary_size, table);
    }

    fclose(f);

    return binary_size;
}
