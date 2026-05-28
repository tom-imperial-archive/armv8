#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "operands.h"
#include "common/instruction.h"
#include "assembler/symbol_table/symbol_table.h"

#define ZERO_REG 31 // TODO - MAYBE FACTOR OUT

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

void apply_register_shift(Instruction *i, char *saveptr) {
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

void build_arithmetic(Instruction *i, OpType immediate_opcode, OpType register_opcode,
                      int rd, int rn, bool sf, char *op2_str, char *saveptr) {
    if (op2_str[0] == '#') {
        // Immediate
        i->op_type = immediate_opcode;
        i->data.immediate_arithmetic.rd = rd;
        i->data.immediate_arithmetic.rn = rn;
        i->data.immediate_arithmetic.sf = sf;
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
        i->data.register_arithmetic_logic.sf = sf;

        bool sf_rm;
        i->data.register_arithmetic_logic.rm = parse_register(op2_str, &sf_rm);

        // Check for shift
        apply_register_shift(i, saveptr);

    }
}

void build_logical(Instruction *i, OpType opcode, int rd, int rn, int rm, bool sf, char *saveptr) {
    i->op_type = opcode;
    i->data.register_arithmetic_logic.rd = rd;
    i->data.register_arithmetic_logic.rn = rn;
    i->data.register_arithmetic_logic.rm = rm;
    i->data.register_arithmetic_logic.sf = sf;

    // Check for shift
    apply_register_shift(i, saveptr);
}

// Intermediate parser for standard arithmetic instructions - add(s) and sub(s)
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

    build_arithmetic(i, immediate_opcode, register_opcode, rd, rn, sf_rd, op2_str, saveptr);
}

// Intermediate parser for compare aliases - cmp and cmn
void parse_compare(char *operands, Instruction *i, OpType immediate_opcode, OpType register_opcode) {
    char *saveptr;
    char *rn_str = strtok_r(operands, " ,", &saveptr);
    char *op2_str = strtok_r(NULL, " ,", &saveptr);

    bool sf_rn;
    int rn = parse_register(rn_str, &sf_rn);

    build_arithmetic(i, immediate_opcode, register_opcode, ZERO_REG, rn, sf_rn, op2_str, saveptr);
}

// Intermediate parser for negate aliases - neg(s)
void parse_negate(char *operands, Instruction *i, OpType immediate_opcode, OpType register_opcode) {
    char *saveptr;
    char *rd_str = strtok_r(operands, " ,", &saveptr);
    char *op2_str = strtok_r(NULL, " ,", &saveptr);

    bool sf_rd;
    int rd = parse_register(rd_str, &sf_rd);

    build_arithmetic(i, immediate_opcode, register_opcode, rd, ZERO_REG, sf_rd, op2_str, saveptr);
}

// Intermediate parser for standard logical operations - and(s), bic(s), eor, orr, eon, and orn
void parse_logical(char *operands, Instruction *i, OpType opcode) {
    char *saveptr;
    char *rd_str = strtok_r(operands, " ,", &saveptr);
    char *rn_str = strtok_r(NULL, " ,", &saveptr);
    char *rm_str = strtok_r(NULL, " ,", &saveptr);

    bool sf_rd, sf_rn, sf_rm;
    int rd = parse_register(rd_str, &sf_rd);
    int rn = parse_register(rn_str, &sf_rn);
    int rm = parse_register(rm_str, &sf_rm);

    if (sf_rd != sf_rn || sf_rd != sf_rm) {
        printf("Error: register size mismatch in logical operation\n");
        exit(EXIT_FAILURE);
    }

    build_logical(i, opcode, rd, rn, rm, sf_rd, saveptr);
}

// Intermediate parser logical aliases - mvn and mov
void parse_move(char *operands, Instruction *i, OpType opcode) {
    char *saveptr;
    char *rd_str = strtok_r(operands, " ,", &saveptr);
    char *rm_str = strtok_r(NULL, " ,", &saveptr);

    bool sf_rd, sf_rm;
    int rd = parse_register(rd_str, &sf_rd);
    int rm = parse_register(rm_str, &sf_rm);

    if (sf_rd != sf_rm) {
        printf("Error: register size mismatch between Rd and Rm\n");
        exit(EXIT_FAILURE);
    }

    build_logical(i, opcode, rd, ZERO_REG, rm, sf_rd, saveptr);
}

void build_multiply(Instruction *i, OpType opcode, int rd, int rn, int rm, int ra, bool sf) {
    i->op_type = opcode;
    i->data.multiply.rd = rd;
    i->data.multiply.rn = rn;
    i->data.multiply.rm = rm;
    i->data.multiply.ra = ra;
    i->data.multiply.sf = sf;
}

void parse_multiply(char *operands, Instruction *i, OpType opcode) {
    char *saveptr;
    char *rd_str = strtok_r(operands, " ,", &saveptr);
    char *rn_str = strtok_r(NULL, " ,", &saveptr);
    char *rm_str = strtok_r(NULL, " ,", &saveptr);
    char *ra_str = strtok_r(NULL, " ,", &saveptr);

    bool sf_rd, sf_rn, sf_rm, sf_ra;
    int rd = parse_register(rd_str, &sf_rd);
    int rn = parse_register(rn_str, &sf_rn);
    int rm = parse_register(rm_str, &sf_rm);
    int ra = parse_register(ra_str, &sf_ra);

    if (sf_rd != sf_rn || sf_rd != sf_rm || sf_rd != sf_ra) {
        printf("Error: register size mismatch in multiply instruction\n");
        exit(EXIT_FAILURE);
    }

    build_multiply(i, opcode, rd, rn, rm, ra, sf_rd);
}

void parse_multiply_alias(char *operands, Instruction *i, OpType opcode) {
    char *saveptr;
    char *rd_str = strtok_r(operands, " ,", &saveptr);
    char *rn_str = strtok_r(NULL, " ,", &saveptr);
    char *rm_str = strtok_r(NULL, " ,", &saveptr);

    bool sf_rd, sf_rn, sf_rm;
    int rd = parse_register(rd_str, &sf_rd);
    int rn = parse_register(rn_str, &sf_rn);
    int rm = parse_register(rm_str, &sf_rm);

    if (sf_rd != sf_rn || sf_rd != sf_rm) {
        printf("Error: register size mismatch in multiply instruction\n");
        exit(EXIT_FAILURE);
    }

    build_multiply(i, opcode, rd, rn, rm, ZERO_REG, sf_rd);
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

void parse_cmp(char *operands, Instruction *i, SymbolTable *table) {
    parse_compare(operands, i, OP_TYPE_SUBS, OP_TYPE_REG_SUBS);
}

void parse_cmn(char *operands, Instruction *i, SymbolTable *table) {
    parse_compare(operands, i, OP_TYPE_ADDS, OP_TYPE_REG_ADDS);
}

void parse_neg(char *operands, Instruction *i, SymbolTable *table) {
    parse_negate(operands, i, OP_TYPE_SUB, OP_TYPE_REG_SUB);
}

void parse_negs(char *operands, Instruction *i, SymbolTable *table) {
    parse_negate(operands, i, OP_TYPE_SUBS, OP_TYPE_REG_SUBS);
}

void parse_and(char *operands, Instruction *i, SymbolTable *table) {
    parse_logical(operands, i, OP_TYPE_AND);
}

void parse_ands(char *operands, Instruction *i, SymbolTable *table) {
    parse_logical(operands, i, OP_TYPE_ANDS);
}

void parse_bic(char *operands, Instruction *i, SymbolTable *table) {
    parse_logical(operands, i, OP_TYPE_BIC);
}

void parse_bics(char *operands, Instruction *i, SymbolTable *table) {
    parse_logical(operands, i, OP_TYPE_BICS);
}

void parse_eor(char *operands, Instruction *i, SymbolTable *table) {
    parse_logical(operands, i, OP_TYPE_EOR);
}

void parse_orr(char *operands, Instruction *i, SymbolTable *table) {
    parse_logical(operands, i, OP_TYPE_ORR);
}

void parse_eon(char *operands, Instruction *i, SymbolTable *table) {
    parse_logical(operands, i, OP_TYPE_EON);
}

void parse_orn(char *operands, Instruction *i, SymbolTable *table) {
    parse_logical(operands, i, OP_TYPE_ORN);
}

// Unique shape so no intermediate parser
void parse_tst(char *operands, Instruction *i, SymbolTable *table) {
    char *saveptr;
    char *rn_str = strtok_r(operands, " ,", &saveptr);
    char *rm_str = strtok_r(NULL, " ,", &saveptr);

    bool sf_rn, sf_rm;
    int rn = parse_register(rn_str, &sf_rn);
    int rm = parse_register(rm_str, &sf_rm);

    if (sf_rn != sf_rm) {
        printf("Error: register size mismatch between Rn and Rm\n");
        exit(EXIT_FAILURE);
    }

    build_logical(i, OP_TYPE_ANDS, ZERO_REG, rn, rm, sf_rn, saveptr);
}

void parse_mvn(char *operands, Instruction *i, SymbolTable *table) {
    parse_move(operands, i, OP_TYPE_ORN);
}

void parse_mov(char *operands, Instruction *i, SymbolTable *table) {
    parse_move(operands, i, OP_TYPE_ORR);
}

void parse_madd(char *operands, Instruction *i, SymbolTable *table) {
    parse_multiply(operands, i, OP_TYPE_MADD);
}

void parse_msub(char *operands, Instruction *i, SymbolTable *table) {
    parse_multiply(operands, i, OP_TYPE_MSUB);
}

void parse_mul(char *operands, Instruction *i, SymbolTable *table) {
    parse_multiply_alias(operands, i, OP_TYPE_MADD);
}

void parse_mneg(char *operands, Instruction *i, SymbolTable *table) {
    parse_multiply_alias(operands, i, OP_TYPE_MSUB);
}

MnemonicMap router[] = {
    {"add", parse_add},
    {"adds", parse_adds},
    {"sub", parse_sub},
    {"subs", parse_subs},
    {"cmp", parse_cmp},
    {"cmn", parse_cmn},
    {"neg", parse_neg},
    {"negs", parse_negs},
    {"and", parse_and},
    {"ands", parse_ands},
    {"bic", parse_bic},
    {"bics", parse_bics},
    {"eor", parse_eor},
    {"orr", parse_orr},
    {"eon", parse_eon},
    {"orn", parse_orn},
    {"tst", parse_tst},
    {"mvn", parse_mvn},
    {"mov", parse_mov},
    {"madd", parse_madd},
    {"msub", parse_msub},
    {"mul", parse_mul},
    {"mneg", parse_mneg},
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
