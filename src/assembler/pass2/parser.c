#include "parser.h"
#include "assembler/symbol_table/symbol_table.h"
#include "common/error.h"
#include "common/instruction.h"
#include "operands.h"
#include "utils/parseutils.h"
#include "utils/types.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define ZERO_REG 31

typedef void (*ParseFunc)(char *, Instruction *, SymbolTable *, uint64);

typedef struct {
    char *mnemonic;
    ParseFunc func;
} MnemonicMap;

// --------------------------------------------------------------
// HELPER FUNCTIONS
// --------------------------------------------------------------
static void apply_register_shift(Instruction *i, char *saveptr) {
    char *shift_type = strtok_r(NULL, " ,\t\n", &saveptr);
    if (shift_type == NULL) {
        // Default behaviour - has no effect
        i->register_arithmetic_logic.shift = SHIFT_LSL;
        i->register_arithmetic_logic.operand = 0;
    } else {
        i->register_arithmetic_logic.shift = parse_shift(shift_type);
        char *shift_amount_str = strtok_r(NULL, " ,\t\n", &saveptr);
        i->register_arithmetic_logic.operand =
            parse_immediate(shift_amount_str);
    }
}

// --------------------------------------------------------------
// BUILD FUNCTIONS
// These functions take the relevant data, and
// populate the instruction struct with it.
// --------------------------------------------------------------
void build_arithmetic(Instruction *i, OpType immediate_opcode,
                      OpType register_opcode, int rd, int rn, bool sf,
                      char *op2_str, char *saveptr) {
    if (op2_str[0] == '#') {
        // Immediate
        i->op_type = immediate_opcode;
        i->immediate_arithmetic.rd = rd;
        i->immediate_arithmetic.rn = rn;
        i->immediate_arithmetic.sf = sf;
        i->immediate_arithmetic.imm12 = parse_immediate(op2_str);

        // Check for shift
        char *shift_type = strtok_r(NULL, " ,\t\n", &saveptr);
        if (shift_type == NULL) {
            i->immediate_arithmetic.sh = 0;
        } else {
            char *shift_amount_str = strtok_r(NULL, " ,\t\n", &saveptr);
            long shift_amount = parse_immediate(shift_amount_str);

            if (shift_amount == 12) {
                i->immediate_arithmetic.sh = 1;
            } else if (shift_amount == 0) {
                i->immediate_arithmetic.sh = 0;
            } else {
                ERROR((Error){.type = DPI_INVALID_SHIFT,
                              .shift_amount = shift_amount});
            }
        }
    } else {
        // Register
        i->op_type = register_opcode;
        i->register_arithmetic_logic.rd = rd;
        i->register_arithmetic_logic.rn = rn;
        i->register_arithmetic_logic.sf = sf;

        bool sf_rm;
        i->register_arithmetic_logic.rm = parse_register(op2_str, &sf_rm);

        // Check for shift
        apply_register_shift(i, saveptr);
    }
}

void build_logical(Instruction *i, OpType opcode, int rd, int rn, int rm,
                   bool sf, char *saveptr) {
    i->op_type = opcode;
    i->register_arithmetic_logic.rd = rd;
    i->register_arithmetic_logic.rn = rn;
    i->register_arithmetic_logic.rm = rm;
    i->register_arithmetic_logic.sf = sf;

    // Check for shift
    apply_register_shift(i, saveptr);
}

void build_multiply(Instruction *i, OpType opcode, int rd, int rn, int rm,
                    int ra, bool sf) {
    i->op_type = opcode;
    i->multiply.rd = rd;
    i->multiply.rn = rn;
    i->multiply.rm = rm;
    i->multiply.ra = ra;
    i->multiply.sf = sf;
}

void build_b(Instruction *i, uint64 offset) {
    i->op_type = OP_TYPE_UNCONDITIONAL_BRANCH;
    i->uncond_branch.simm26 = offset;
}

void build_br(Instruction *i, int xn) {
    i->op_type = OP_TYPE_BR;
    i->reg_branch.xn = xn;
}

void build_b_cond(Instruction *i, uint64 offset, OpType opcode) {
    i->op_type = opcode;
    i->cond_branch.simm19 = offset;
}

void build_literal(Instruction *i, int rt, bool sf, int64 offset) {
    i->op_type = OP_TYPE_LOAD_LITERAL;
    i->load_literal.rt = rt;
    i->load_literal.sf = sf;
    i->load_literal.simm19 = offset;
}

void build_single_data_transfer(Instruction *i, OpType opcode, int rt, bool sf,
                                bool is_load, AddressingMode mode, int xm,
                                int64 offset) {
    i->op_type = opcode;
    i->single_data_transfer.rt = rt;
    i->single_data_transfer.sf = sf;
    i->single_data_transfer.L = is_load;
    i->single_data_transfer.mode = mode;
    i->single_data_transfer.xm = xm;
    i->single_data_transfer.offset = offset;
}

void build_wide_move(Instruction *i, OpType opcode, bool sf, int rd,
                     int64 imm16, int hw) {
    i->op_type = opcode;
    i->wide_move.sf = sf;
    i->wide_move.rd = rd;
    i->wide_move.imm16 = imm16;
    i->wide_move.hw = hw;
}

void build_directive(Instruction *i, int value) {
    i->op_type = OP_TYPE_DIRECTIVE_INT;
    i->directive_int.value = value;
}

// --------------------------------------------------------------
// INTERMEDIATE PARSING FUNCTIONS
// These are called as an intermediate step between the initial parse function
// and the final build function, and handle multiple highly similar
// instructions.
// --------------------------------------------------------------

// Intermediate parser for standard arithmetic instructions - add(s) and sub(s)
void parse_arithmetic(char *operands, Instruction *i, OpType immediate_opcode,
                      OpType register_opcode) {
    char *saveptr;
    char *rd_str = strtok_r(operands, " ,", &saveptr);
    char *rn_str = strtok_r(NULL, " ,", &saveptr);
    char *op2_str = strtok_r(NULL, " ,\t\n", &saveptr);

    bool sf_rd, sf_rn;
    int rd = parse_register(rd_str, &sf_rd);
    int rn = parse_register(rn_str, &sf_rn);

    if (sf_rd != sf_rn) {
        ERROR((Error){.type = REGISTER_SIZE_MISMATCH});
    }

    build_arithmetic(i, immediate_opcode, register_opcode, rd, rn, sf_rd,
                     op2_str, saveptr);
}

// Intermediate parser for compare aliases - cmp and cmn
void parse_compare(char *operands, Instruction *i, OpType immediate_opcode,
                   OpType register_opcode) {
    char *saveptr;
    char *rn_str = strtok_r(operands, " ,", &saveptr);
    char *op2_str = strtok_r(NULL, " ,\t\n", &saveptr);

    bool sf_rn;
    int rn = parse_register(rn_str, &sf_rn);

    build_arithmetic(i, immediate_opcode, register_opcode, ZERO_REG, rn, sf_rn,
                     op2_str, saveptr);
}

// Intermediate parser for negate aliases - neg(s)
void parse_negate(char *operands, Instruction *i, OpType immediate_opcode,
                  OpType register_opcode) {
    char *saveptr;
    char *rd_str = strtok_r(operands, " ,", &saveptr);
    char *op2_str = strtok_r(NULL, " ,\t\n", &saveptr);

    bool sf_rd;
    int rd = parse_register(rd_str, &sf_rd);

    build_arithmetic(i, immediate_opcode, register_opcode, rd, ZERO_REG, sf_rd,
                     op2_str, saveptr);
}

// Intermediate parser for standard logical operations - and(s), bic(s), eor,
// orr, eon, and orn
void parse_logical(char *operands, Instruction *i, OpType opcode) {
    char *saveptr;
    char *rd_str = strtok_r(operands, " ,", &saveptr);
    char *rn_str = strtok_r(NULL, " ,", &saveptr);
    char *rm_str = strtok_r(NULL, " ,\t\n", &saveptr);

    bool sf_rd, sf_rn, sf_rm;
    int rd = parse_register(rd_str, &sf_rd);
    int rn = parse_register(rn_str, &sf_rn);
    int rm = parse_register(rm_str, &sf_rm);

    if (sf_rd != sf_rn || sf_rd != sf_rm) {
        ERROR((Error){.type = REGISTER_SIZE_MISMATCH});
    }

    build_logical(i, opcode, rd, rn, rm, sf_rd, saveptr);
}

// Intermediate parser logical aliases - mvn and mov
void parse_move(char *operands, Instruction *i, OpType opcode) {
    char *saveptr;
    char *rd_str = strtok_r(operands, " ,", &saveptr);
    char *rm_str = strtok_r(NULL, " ,\t\n", &saveptr);

    bool sf_rd, sf_rm;
    int rd = parse_register(rd_str, &sf_rd);
    int rm = parse_register(rm_str, &sf_rm);

    if (sf_rd != sf_rm) {
        ERROR((Error){.type = REGISTER_SIZE_MISMATCH});
    }

    build_logical(i, opcode, rd, ZERO_REG, rm, sf_rd, saveptr);
}

// Intermediate parser for multiply instructions - madd and msub
void parse_multiply(char *operands, Instruction *i, OpType opcode) {
    char *saveptr;
    char *rd_str = strtok_r(operands, " ,", &saveptr);
    char *rn_str = strtok_r(NULL, " ,", &saveptr);
    char *rm_str = strtok_r(NULL, " ,", &saveptr);
    char *ra_str = strtok_r(NULL, " ,\t\n", &saveptr);

    bool sf_rd, sf_rn, sf_rm, sf_ra;
    int rd = parse_register(rd_str, &sf_rd);
    int rn = parse_register(rn_str, &sf_rn);
    int rm = parse_register(rm_str, &sf_rm);
    int ra = parse_register(ra_str, &sf_ra);

    if (sf_rd != sf_rn || sf_rd != sf_rm || sf_rd != sf_ra) {
        ERROR((Error){.type = REGISTER_SIZE_MISMATCH});
    }

    build_multiply(i, opcode, rd, rn, rm, ra, sf_rd);
}

// Intermediate parser for multiply alias instructions - mul and mneg
void parse_multiply_alias(char *operands, Instruction *i, OpType opcode) {
    char *saveptr;
    char *rd_str = strtok_r(operands, " ,", &saveptr);
    char *rn_str = strtok_r(NULL, " ,", &saveptr);
    char *rm_str = strtok_r(NULL, " ,\t\n", &saveptr);

    bool sf_rd, sf_rn, sf_rm;
    int rd = parse_register(rd_str, &sf_rd);
    int rn = parse_register(rn_str, &sf_rn);
    int rm = parse_register(rm_str, &sf_rm);

    if (sf_rd != sf_rn || sf_rd != sf_rm) {
        ERROR((Error){.type = REGISTER_SIZE_MISMATCH});
    }

    build_multiply(i, opcode, rd, rn, rm, ZERO_REG, sf_rd);
}

// Intermediate parser for conditional branch instructions - b.eq, b.ne, b.ge,
// b.lt, b.gt, b.le, and b.al
void parse_b_cond(char *operands, Instruction *i, SymbolTable *table,
                  uint64 current_pc, OpType opcode) {
    char *saveptr;
    char *label = strtok_r(operands, " \t\n", &saveptr);
    uint64 target = symbol_table_lookup(table, label);

    int64 offset = calculate_offset(current_pc, target);

    build_b_cond(i, offset, opcode);
}

// Intermediate parser for memory instructions - ldr and str
void parse_memory(char *operands, Instruction *i, SymbolTable *table,
                  uint64 current_pc, bool is_load) {
    char *saveptr;

    char *rt_str = strtok_r(operands, " ,\t\n", &saveptr);
    bool sf_rt;
    int rt = parse_register(rt_str, &sf_rt);

    // Strip leading whitespace
    saveptr = trim_leading_whitespace(saveptr);
    char *address_str = saveptr;
    // Strip trailing newlines
    address_str[strcspn(address_str, "\n")] = '\0';
    // Literal
    if (strchr(address_str, '[') == NULL) {
        if (!is_load) {
            ERROR((Error){.type = ILLEGAL_STR_ADDRESSING, .str = address_str});
        }
        uint64 target;
        if (address_str[0] == '#') {
            target = parse_immediate(address_str);
        } else {
            target = symbol_table_lookup(table, address_str);
        }
        int64 offset = calculate_offset(current_pc, target);

        build_literal(i, rt, sf_rt, offset);
        return;
    }

    // Single data transfer
    bool is_pre_indexed = (strchr(address_str, '!') != NULL);
    bool is_post_indexed = (strstr(address_str, "],") != NULL);

    char *token_ptr;
    char *xn_str = strtok_r(address_str, "[],! \t\n", &token_ptr);
    char *op2_str = strtok_r(NULL, "[],! \t\n", &token_ptr);

    bool sf_xn;
    i->single_data_transfer.xn = parse_register(xn_str, &sf_xn);

    // Register offset
    if (op2_str != NULL && op2_str[0] != '#') {
        bool sf_xm;
        int xm = parse_register(op2_str, &sf_xm);
        build_single_data_transfer(i, OP_TYPE_SINGLE_DATA_TRANSFER, rt, sf_rt,
                                   is_load, ADDR_REGISTER_OFFSET, xm, 0);
    } else {
        // Immediate offset - Pre, Post, or Unsigned
        long imm = (op2_str != NULL) ? parse_immediate(op2_str) : 0;

        if (is_pre_indexed) {
            build_single_data_transfer(i, OP_TYPE_SINGLE_DATA_TRANSFER, rt,
                                       sf_rt, is_load, ADDR_PRE_INDEXED, 0,
                                       imm);
        } else if (is_post_indexed) {
            build_single_data_transfer(i, OP_TYPE_SINGLE_DATA_TRANSFER, rt,
                                       sf_rt, is_load, ADDR_POST_INDEXED, 0,
                                       imm);
        } else {
            // Scaling rules
            int scale = sf_rt ? 8 : 4;
            if (imm % scale != 0) {
                ERROR((Error){.type = OFFSET_MULTIPLE_N, .index = scale});
            }
            int64 offset = imm / scale;
            build_single_data_transfer(i, OP_TYPE_SINGLE_DATA_TRANSFER, rt,
                                       sf_rt, is_load, ADDR_UNSIGNED_OFFSET, 0,
                                       offset);
        }
    }
}

// Intermediate parser for wide move instructions - movk, movn, and movz
void parse_wide_move(char *operands, Instruction *i, OpType opcode) {
    char *saveptr;

    char *rd_str = strtok_r(operands, " ,", &saveptr);
    bool sf_rd;
    int rd = parse_register(rd_str, &sf_rd);

    char *imm_str = strtok_r(NULL, " ,", &saveptr);
    int64 imm16 = parse_immediate(imm_str);

    if (imm16 < 0 || imm16 > 0xFFFF) {
        ERROR((Error){.type = ILLEGAL_WIDE_MOVE_SIZE, .shift_amount = imm16});
    }

    int hw = 0;
    char *lsl_str = strtok_r(NULL, " ,", &saveptr);
    if (lsl_str != NULL) {
        if (strcmp(lsl_str, "lsl") != 0 && strcmp(lsl_str, "LSL") != 0) {
            ERROR((Error){.type = WIDE_MOVE_REQUIRES_LSL});
        }

        char *shift_amount_str = strtok_r(NULL, " ,\t\n", &saveptr);
        long shift_amount = parse_immediate(shift_amount_str);

        if (shift_amount == 0) {
            hw = 0;
        } else if (shift_amount == 16) {
            hw = 1;
        } else if (shift_amount == 32 && sf_rd) { // 32 is only valid for 64-bit
            hw = 2;
        } else if (shift_amount == 48 && sf_rd) { // 48 is only valid for 64-bit
            hw = 3;
        } else {
            ERROR((Error){.type = INVALID_SHIFT_AMOUNT,
                          .shift_amount = shift_amount});
        }
    }

    build_wide_move(i, opcode, sf_rd, rd, imm16, hw);
}

// --------------------------------------------------------------
// INDIVIDUAL PARSING FUNCTIONS
// There is a single function for each possible instruction.
// Most simply call an intermediate function, providing an opcode or similar.
// Some are unique with no intermediate, so feature a small amount of parsing
//     logic before directly calling a building function.
// --------------------------------------------------------------

void parse_add(char *operands, Instruction *i, SymbolTable *table,
               uint64 current_pc) {
    parse_arithmetic(operands, i, OP_TYPE_ADD, OP_TYPE_REG_ADD);
}

void parse_adds(char *operands, Instruction *i, SymbolTable *table,
                uint64 current_pc) {
    parse_arithmetic(operands, i, OP_TYPE_ADDS, OP_TYPE_REG_ADDS);
}

void parse_sub(char *operands, Instruction *i, SymbolTable *table,
               uint64 current_pc) {
    parse_arithmetic(operands, i, OP_TYPE_SUB, OP_TYPE_REG_SUB);
}

void parse_subs(char *operands, Instruction *i, SymbolTable *table,
                uint64 current_pc) {
    parse_arithmetic(operands, i, OP_TYPE_SUBS, OP_TYPE_REG_SUBS);
}

void parse_cmp(char *operands, Instruction *i, SymbolTable *table,
               uint64 current_pc) {
    parse_compare(operands, i, OP_TYPE_SUBS, OP_TYPE_REG_SUBS);
}

void parse_cmn(char *operands, Instruction *i, SymbolTable *table,
               uint64 current_pc) {
    parse_compare(operands, i, OP_TYPE_ADDS, OP_TYPE_REG_ADDS);
}

void parse_neg(char *operands, Instruction *i, SymbolTable *table,
               uint64 current_pc) {
    parse_negate(operands, i, OP_TYPE_SUB, OP_TYPE_REG_SUB);
}

void parse_negs(char *operands, Instruction *i, SymbolTable *table,
                uint64 current_pc) {
    parse_negate(operands, i, OP_TYPE_SUBS, OP_TYPE_REG_SUBS);
}

void parse_and(char *operands, Instruction *i, SymbolTable *table,
               uint64 current_pc) {
    parse_logical(operands, i, OP_TYPE_AND);
}

void parse_ands(char *operands, Instruction *i, SymbolTable *table,
                uint64 current_pc) {
    parse_logical(operands, i, OP_TYPE_ANDS);
}

void parse_bic(char *operands, Instruction *i, SymbolTable *table,
               uint64 current_pc) {
    parse_logical(operands, i, OP_TYPE_BIC);
}

void parse_bics(char *operands, Instruction *i, SymbolTable *table,
                uint64 current_pc) {
    parse_logical(operands, i, OP_TYPE_BICS);
}

void parse_eor(char *operands, Instruction *i, SymbolTable *table,
               uint64 current_pc) {
    parse_logical(operands, i, OP_TYPE_EOR);
}

void parse_orr(char *operands, Instruction *i, SymbolTable *table,
               uint64 current_pc) {
    parse_logical(operands, i, OP_TYPE_ORR);
}

void parse_eon(char *operands, Instruction *i, SymbolTable *table,
               uint64 current_pc) {
    parse_logical(operands, i, OP_TYPE_EON);
}

void parse_orn(char *operands, Instruction *i, SymbolTable *table,
               uint64 current_pc) {
    parse_logical(operands, i, OP_TYPE_ORN);
}

void parse_tst(char *operands, Instruction *i, SymbolTable *table,
               uint64 current_pc) {
    char *saveptr;
    char *rn_str = strtok_r(operands, " ,", &saveptr);
    char *rm_str = strtok_r(NULL, " ,", &saveptr);

    bool sf_rn, sf_rm;
    int rn = parse_register(rn_str, &sf_rn);
    int rm = parse_register(rm_str, &sf_rm);

    if (sf_rn != sf_rm) {
        ERROR((Error){.type = REGISTER_SIZE_MISMATCH});
    }

    build_logical(i, OP_TYPE_ANDS, ZERO_REG, rn, rm, sf_rn, saveptr);
}

void parse_mvn(char *operands, Instruction *i, SymbolTable *table,
               uint64 current_pc) {
    parse_move(operands, i, OP_TYPE_ORN);
}

void parse_mov(char *operands, Instruction *i, SymbolTable *table,
               uint64 current_pc) {
    parse_move(operands, i, OP_TYPE_ORR);
}

void parse_madd(char *operands, Instruction *i, SymbolTable *table,
                uint64 current_pc) {
    parse_multiply(operands, i, OP_TYPE_MADD);
}

void parse_msub(char *operands, Instruction *i, SymbolTable *table,
                uint64 current_pc) {
    parse_multiply(operands, i, OP_TYPE_MSUB);
}

void parse_mul(char *operands, Instruction *i, SymbolTable *table,
               uint64 current_pc) {
    parse_multiply_alias(operands, i, OP_TYPE_MADD);
}

void parse_mneg(char *operands, Instruction *i, SymbolTable *table,
                uint64 current_pc) {
    parse_multiply_alias(operands, i, OP_TYPE_MSUB);
}

void parse_b(char *operands, Instruction *i, SymbolTable *table,
             uint64 current_pc) {
    char *saveptr;
    char *label = strtok_r(operands, " \t\n", &saveptr);
    uint64 target = symbol_table_lookup(table, label);

    int64 offset = calculate_offset(current_pc, target);

    build_b(i, offset);
}

void parse_br(char *operands, Instruction *i, SymbolTable *table,
              uint64 current_pc) {
    char *saveptr;
    char *xn_str = strtok_r(operands, " ,\t\n", &saveptr);

    bool is_64_bit;
    int xn = parse_register(xn_str, &is_64_bit);

    build_br(i, xn);
}

void parse_b_eq(char *operands, Instruction *i, SymbolTable *table,
                uint64 current_pc) {
    parse_b_cond(operands, i, table, current_pc, OP_TYPE_EQ);
}

void parse_b_ne(char *operands, Instruction *i, SymbolTable *table,
                uint64 current_pc) {
    parse_b_cond(operands, i, table, current_pc, OP_TYPE_NE);
}

void parse_b_ge(char *operands, Instruction *i, SymbolTable *table,
                uint64 current_pc) {
    parse_b_cond(operands, i, table, current_pc, OP_TYPE_GE);
}

void parse_b_lt(char *operands, Instruction *i, SymbolTable *table,
                uint64 current_pc) {
    parse_b_cond(operands, i, table, current_pc, OP_TYPE_LT);
}

void parse_b_gt(char *operands, Instruction *i, SymbolTable *table,
                uint64 current_pc) {
    parse_b_cond(operands, i, table, current_pc, OP_TYPE_GT);
}

void parse_b_le(char *operands, Instruction *i, SymbolTable *table,
                uint64 current_pc) {
    parse_b_cond(operands, i, table, current_pc, OP_TYPE_LE);
}

void parse_b_al(char *operands, Instruction *i, SymbolTable *table,
                uint64 current_pc) {
    parse_b_cond(operands, i, table, current_pc, OP_TYPE_AL);
}

void parse_ldr(char *operands, Instruction *i, SymbolTable *table,
               uint64_t current_pc) {
    parse_memory(operands, i, table, current_pc, true);
}

void parse_str(char *operands, Instruction *i, SymbolTable *table,
               uint64_t current_pc) {
    parse_memory(operands, i, table, current_pc, false);
}

void parse_movk(char *operands, Instruction *i, SymbolTable *table,
                uint64 current_pc) {
    parse_wide_move(operands, i, OP_TYPE_MOVK);
}

void parse_movn(char *operands, Instruction *i, SymbolTable *table,
                uint64 current_pc) {
    parse_wide_move(operands, i, OP_TYPE_MOVN);
}

void parse_movz(char *operands, Instruction *i, SymbolTable *table,
                uint64 current_pc) {
    parse_wide_move(operands, i, OP_TYPE_MOVZ);
}

void parse_directive_int(char *operands, Instruction *i, SymbolTable *table,
                         uint64 current_pc) {
    char *saveptr;
    char *val_str = strtok_r(operands, " \t\n", &saveptr);

    if (val_str == NULL) {
        ERROR((Error){.type = INT_DIRECTIVE_REQUIRES_VALUE});
    }

    // parse_immediate() is unsuitable here, since directives do not use the #
    // prefix
    long val = strtol(val_str, NULL, 0);

    build_directive(i, (int)val);
}

// --------------------------------------------------------------
// ROUTING TABLE
// Stores each instruction's corresponding parsing function.
// --------------------------------------------------------------
MnemonicMap router[] = {
    {"add", parse_add},   {"adds", parse_adds},
    {"sub", parse_sub},   {"subs", parse_subs},
    {"cmp", parse_cmp},   {"cmn", parse_cmn},
    {"neg", parse_neg},   {"negs", parse_negs},
    {"and", parse_and},   {"ands", parse_ands},
    {"bic", parse_bic},   {"bics", parse_bics},
    {"eor", parse_eor},   {"orr", parse_orr},
    {"eon", parse_eon},   {"orn", parse_orn},
    {"tst", parse_tst},   {"mvn", parse_mvn},
    {"mov", parse_mov},   {"madd", parse_madd},
    {"msub", parse_msub}, {"mul", parse_mul},
    {"mneg", parse_mneg}, {"b", parse_b},
    {"br", parse_br},     {"b.eq", parse_b_eq},
    {"b.ne", parse_b_ne}, {"b.ge", parse_b_ge},
    {"b.lt", parse_b_lt}, {"b.gt", parse_b_gt},
    {"b.le", parse_b_le}, {"b.al", parse_b_al},
    {"ldr", parse_ldr},   {"str", parse_str},
    {"movk", parse_movk}, {"movn", parse_movn},
    {"movz", parse_movz}, {".int", parse_directive_int},
};

// Takes a single line of assembly, an empty instruction struct,
//     a SymbolTable, and the current program counter
// If it's an instruction, we populate the instruction struct and return true.
// If the line is blank, a comment, or a label, we simply return false
bool parse_line(char *line, Instruction *i, SymbolTable *table,
                uint64 current_pc) {
    char *saveptr;

    // Take the first word (the mnemonic or label)
    char *mnemonic = strtok_r(line, " \t\n", &saveptr);
    if (mnemonic == NULL) {
        return false;
    }

    // If the first word is a label, there may be an instruction on the same
    // line
    if (mnemonic[strlen(mnemonic) - 1] == ':') {
        // Inspect next word on line
        mnemonic = strtok_r(NULL, " \t\n", &saveptr);
        if (mnemonic == NULL) {
            return false;
        }
    }

    // Use the routing table to call the correct function
    for (int cnt = 0; cnt < (sizeof(router) / sizeof(MnemonicMap)); cnt++) {
        if (!strcmp(mnemonic, router[cnt].mnemonic)) {
            router[cnt].func(saveptr, i, table, current_pc);

            return true;
        }
    }
    // Unrecognised instruction
    ERROR((Error){.type = UNKNOWN_MNENOMIC, .str = mnemonic});
    return false;
}
