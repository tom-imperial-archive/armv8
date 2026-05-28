#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <regex.h>
#include <string.h>
#include <assert.h>
#include "assembler/symbol_table/symbol_table.h"
#include "utils/types.h"
#define INSTRUCTION_SIZE 4
#define MAX_FILE_LINE_LENGTH 120
#define REGEX_FLAGS (REG_EXTENDED | REG_ICASE)
/*
THIS IS PASS 1 OF THE TWO PASS APPROACH

We read through each line, tracking the current memory address (making sure to ignore comments).
When we enccounter a label, we add it, and the current address, to the symbol table.

This accepts any preceeding whitespace
*/

// Regex: [a-zA-Z_\.]([a-zA-Z0-9$_\.])*
static void read_line(char *buf, uint64 *address, SymbolTable *table)
{
    // Remove whitespace
    while (*buf == '\t' || *buf == ' ') buf++;

    regex_t labelRegex;
    const char labelExp[] = "^[a-zA-Z_.][a-zA-Z0-9_$.]*:";
    int labelRes = regcomp(&labelRegex, labelExp, REGEX_FLAGS);

    regex_t directiveRegex;
    const char directiveExp[] = "^[.]([a-zA-Z0-9]+)";
    int directiveRes = regcomp(&directiveRegex, directiveExp, REG_EXTENDED);

    // Assert compilation was successful
    assert(labelRes == 0);
    assert(directiveRes == 0);

    //printf("Str: %s", buf);
    regmatch_t match[1];

    // Check for a label
    if (regexec(&labelRegex, buf, 1, match, 0) == 0)
    {
        // This is a label
        // Find the length of the matching substring. We remove the trailing : so subtract 1 from the length.
        int length = match[0].rm_eo - match[0].rm_so - 1;
        char label[length + 1];
        strncpy(label, buf + match[0].rm_so, length);
        label[length] = '\0';

        symbol_table_add(table, label, *address);
    } else if (buf[0] != '\0') {
        // Line is non-empty. We either have a directive or an instruction. Either way, this corresponds to 4 bytes once assembled
        *address += 4;
    }

    regfree(&labelRegex);
    regfree(&directiveRegex);
}

// This should read through the file, and populate a SymbolTable mapping labels to addresses
// Note this is def not the best way to pass the file around, but it's sufficient for this sketch.
void scan_file(char *filename, SymbolTable *table)
{
    FILE *f = fopen(filename, "rb");
    if (f == NULL)
    {
        printf("Failed to open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    char buf[MAX_FILE_LINE_LENGTH];

    uint64 address = 0;
    while (fgets(buf, MAX_FILE_LINE_LENGTH, f))
    {
        read_line(buf, &address, table);
    }

    return;
}
