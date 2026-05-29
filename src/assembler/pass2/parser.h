#ifndef PARSER_H
#define PARSER_H

#include "common/instruction.h"
#include "assembler/symbol_table/symbol_table.h"
#include "stdbool.h"

// Takes a single line of assembly
// If it's an instruction, we populate the instruction struct and return true.
// If the line is blank, a comment, or a label, we simply return false.
bool parse_line(char *line, Instruction *i, SymbolTable *table, uint64 current_pc);

#endif
