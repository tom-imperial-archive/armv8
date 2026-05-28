#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "operands.h"
#include "common/instruction.h"
#include "assembler/symbol_table/symbol_table.h"

/*
THIS IS PASS 2 OF THE TWO PASS APPROACH

Here, we tokenise each line in the file, and build it into a struct
We are reusing the same structs as we did in deocde (now stored in common/instruction.h)
Note operands.c provides helpers that we should use here.

We should use function pointers here (as suggested in the spec) to avoid a very messy if/then/else structure.
*/

typedef void (*ParseFunc)(char *, Instruction *, SymbolTable *);

typedef struct {
    char *mnemonic;
    ParseFunc func;
} MnemonicMap;

void parse_arithmetic(char *operands, Instruction *i, OpType immediate_opcode, OpType register_opcode) {
    char *saveptr;
    char *rd_str = strtok_r(operands, " ,", &saveptr);
    char *rn_str = strtok_r(NULL, " ,", &saveptr);
    char *op2_str = strtok_r(NULL, " ,", &saveptr);

    bool sf_rd, sf_rn;
    int rd = parse_register(rd_str, &sf_rd);
    int rn = parse_register(rn_str, &sf_rn);

    if (sf_rd != sf_rn) {
        printf("Error: register size mismatch between Rd and Rn\n");
        exit(EXIT_FAILURE);
    }

    if (op2_str[0] == '#') {
        // Immediate
        i->op_type = immediate_opcode;
        i->data.immediate_arithmetic.rd = rd;
        i->data.immediate_arithmetic.rn = rn;
        i->data.immediate_arithmetic.sf = sf_rd;
        i->data.immediate_arithmetic.imm12 = parse_immediate(op2_str);

        // Check for shift
        char *shift_type = strtok_r(NULL, " ,\t\n", &saveptr);
        if (shift_type == NULL) {
            i->data.immediate_arithmetic.sh = 0;
        } else {
            char *shift_amount_str = strtok_r(NULL, " ,\t\n", &saveptr);
            long shift_amount = parse_immediate(shift_amount_str);

            if (shift_amount == 12) {
                i->data.immediate_arithmetic.sh = 1;
            } else if (shift_amount == 0) {
                i->data.immediate_arithmetic.sh = 0;
            } else {
                printf("Error: DPI shift must be 0 or 12\n");
                exit(EXIT_FAILURE);
            }
        }
    } else {
        // Register
        i->op_type = register_opcode;
        i->data.register_arithmetic_logic.rd = rd;
        i->data.register_arithmetic_logic.rn = rn;
        i->data.register_arithmetic_logic.sf = sf_rd;

        bool sf_rm;
        i->data.register_arithmetic_logic.rm = parse_register(op2_str, &sf_rm);

        // Check for shift
        char *shift_type = strtok_r(NULL, " ,\t\n", &saveptr);
        if (shift_type == NULL) {
            // Default behaviour - has no effect
            i->data.register_arithmetic_logic.shift = SHIFT_LSL;
            i->data.register_arithmetic_logic.operand = 0;
        } else {
            i->data.register_arithmetic_logic.shift = parse_shift(shift_type);
            char *shift_amount_str = strtok_r(NULL, " ,\t\n", &saveptr);
            i->data.register_arithmetic_logic.operand = parse_immediate(shift_amount_str);
        }

    }
}

void parse_add(char *operands, Instruction *i, SymbolTable *table) {
    parse_arithmetic(operands, i, OP_TYPE_ADD, OP_TYPE_REG_ADD);
}

void parse_adds(char *operands, Instruction *i, SymbolTable *table) {
    parse_arithmetic(operands, i, OP_TYPE_ADDS, OP_TYPE_REG_ADDS);
}

void parse_sub(char *operands, Instruction *i, SymbolTable *table) {
    parse_arithmetic(operands, i, OP_TYPE_SUB, OP_TYPE_REG_SUB);
}

void parse_subs(char *operands, Instruction *i, SymbolTable *table) {
    parse_arithmetic(operands, i, OP_TYPE_SUBS, OP_TYPE_REG_SUBS);
}

MnemonicMap router[] = {
    {"add", parse_add},
    {"adds", parse_adds},
    {"sub", parse_sub},
    {"subs", parse_subs},
};

// Takes a single line of assembly
// If it's an instruction, we populate the instruction struct and return true.
// If the line is blank, a comment, or a label, we simply return false.
bool parse_line(char *line, Instruction *i, SymbolTable *table) {
    char *saveptr;

    // Take the first word (the mnemonic or label)
    char *mnemonic = strtok_r(line, " \t\n", &saveptr);

    if (mnemonic == NULL || mnemonic[strlen(mnemonic)-1] == ':') {
        // Blank line or label, so ignore
        return false;
    }

    // Use the routing table to call the correct function
    for (int cnt = 0; cnt < (sizeof(router)/sizeof(MnemonicMap)); cnt++){
        if (!strcmp(mnemonic, router[cnt].mnemonic)) {
            router[cnt].func(saveptr, i, table);

            return true;
        }
    }
    // Unrecognised instruction
    printf("Error: Unknown mnemonic '%s'\n", mnemonic);
    return false;
}
