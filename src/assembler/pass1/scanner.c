#include "assembler/symbol_table/symbol_table.h"
#include "utils/parseutils.h"
#include "utils/types.h"
#include <assert.h>
#include <ctype.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>

#define is_label_char(c)                                                       \
    isalnum(c) || c == '_' || c == '.' || c == '\\' || c == '$'

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

static void read_line(char *buf, uint64 *address, SymbolTable *table) {
    // Remove whitespace
    buf = trim_leading_whitespace(buf);

    if (*buf == '\0') {
        // Line is just whitespace
        return;
    }

    // Line is not whitespace
    int i = 0;
    while (is_label_char(buf[i])) {
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

/*
This populates the symbol table table with the assembly from the file.
The file pointer is returned to the beginning of the file.
*/
void scan_file(FILE *file, SymbolTable *table) {
    char buf[MAX_FILE_LINE_LENGTH];

    uint64 address = 0;
    while (fgets(buf, MAX_FILE_LINE_LENGTH, file)) {
        read_line(buf, &address, table);
    }

    rewind(file);
}
