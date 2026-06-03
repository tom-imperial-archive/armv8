#include "instruction.h"
#include "utils/types.h"
#include <stdbool.h>

bool is_immediate_arithmetic(OpType op_type) {
    return op_type == OP_TYPE_ADD || op_type == OP_TYPE_ADDS ||
           op_type == OP_TYPE_SUB || op_type == OP_TYPE_SUBS;
}

bool is_wide_move(OpType op_type) {
    return op_type == OP_TYPE_MOVN || op_type == OP_TYPE_MOVZ ||
           op_type == OP_TYPE_MOVK;
}

bool is_register_arithmetic_logic(OpType op_type) {
    return op_type == OP_TYPE_REG_ADD || op_type == OP_TYPE_REG_ADDS ||
           op_type == OP_TYPE_REG_SUB || op_type == OP_TYPE_REG_SUBS ||
           op_type == OP_TYPE_AND || op_type == OP_TYPE_BIC ||
           op_type == OP_TYPE_ORR || op_type == OP_TYPE_ORN ||
           op_type == OP_TYPE_EOR || op_type == OP_TYPE_EON ||
           op_type == OP_TYPE_ANDS || op_type == OP_TYPE_BICS;
}

bool is_register_multiply(OpType op_type) {
    return op_type == OP_TYPE_MADD || op_type == OP_TYPE_MSUB;
}

bool is_single_data_transfer(OpType op_type) {
    return op_type == OP_TYPE_SINGLE_DATA_TRANSFER;
}

bool is_load_literal(OpType op_type) { return op_type == OP_TYPE_LOAD_LITERAL; }

bool is_conditional_branch(OpType op_type) {
    return op_type == OP_TYPE_EQ || op_type == OP_TYPE_NE ||
           op_type == OP_TYPE_GE || op_type == OP_TYPE_LT ||
           op_type == OP_TYPE_GT || op_type == OP_TYPE_LE ||
           op_type == OP_TYPE_AL;
}

bool is_unconditional_branch(OpType op_type) {
    return op_type == OP_TYPE_UNCONDITIONAL_BRANCH;
}

bool is_register_branch(OpType op_type) { return op_type == OP_TYPE_BR; }

bool is_branch(OpType op_type) {
    return is_conditional_branch(op_type) || is_unconditional_branch(op_type) || is_register_branch(op_type);
}

bool is_directive_int(OpType op_type) {
    return op_type == OP_TYPE_DIRECTIVE_INT;
}
