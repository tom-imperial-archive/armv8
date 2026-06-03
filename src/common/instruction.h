#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "utils/types.h"
#include <stdbool.h>

typedef enum OpType {
    // Halt
    OP_TYPE_HALT,

    // Data processing instruction (immediate). Correspond to
    // `ImmediateArithmeticInstruction`.

    // Arithmetic
    OP_TYPE_ADD,
    OP_TYPE_ADDS,
    OP_TYPE_SUB,
    OP_TYPE_SUBS,

    // Wide move
    OP_TYPE_MOVN,
    OP_TYPE_MOVZ,
    OP_TYPE_MOVK,

    // Data processing instruction (register). Correspond to
    // `RegisterArithmeticLogicInstruction`

    // Arithmetic
    OP_TYPE_REG_ADD,
    OP_TYPE_REG_ADDS,
    OP_TYPE_REG_SUB,
    OP_TYPE_REG_SUBS,

    // Logic
    OP_TYPE_AND,
    OP_TYPE_BIC,
    OP_TYPE_ORR,
    OP_TYPE_ORN,
    OP_TYPE_EOR,
    OP_TYPE_EON,
    OP_TYPE_ANDS,
    OP_TYPE_BICS,

    // Multiply. Correspond to `RegisterMultiplyInstruction`
    OP_TYPE_MADD,
    OP_TYPE_MSUB,

    // Single data transfer instructions
    OP_TYPE_SINGLE_DATA_TRANSFER,
    OP_TYPE_LOAD_LITERAL,
    // Branch instructions
    OP_TYPE_EQ,
    OP_TYPE_NE,
    OP_TYPE_GE,
    OP_TYPE_LT,
    OP_TYPE_GT,
    OP_TYPE_LE,
    OP_TYPE_AL,
    OP_TYPE_BR,
    OP_TYPE_UNCONDITIONAL_BRANCH,

    // Directive
    OP_TYPE_DIRECTIVE_INT,
} OpType;

// Data Processing Instruction (Immediate)

typedef struct ImmediateArithmeticInstruction {
    uint16 imm12;
    int rd;
    int rn;
    bool sf;
    bool sh;
} ImmediateArithmeticInstruction;

typedef struct WideMoveInstruction {
    int hw;
    int imm16;
    int rd;
    bool sf;
} WideMoveInstruction;

// Data Processing Instruction (Register)

typedef struct RegisterMultiplyInstruction {
    int rm;
    int rn;
    int rd;
    int ra;
    bool sf;
} RegisterMultiplyInstruction;

typedef enum ShiftType {
    SHIFT_LSL,
    SHIFT_LSR,
    SHIFT_ASR,
    SHIFT_ROR,
} ShiftType;

typedef struct RegisterArithmeticLogicInstruction {
    int rd;
    int rn;
    int rm;
    int operand;
    ShiftType shift;
    bool sf;
} RegisterArithmeticLogicInstruction;

// Branch Instruction

typedef struct UnconditionalBranchInstruction {
    int simm26;
} UnconditionalBranchInstruction;

typedef struct RegisterBranchInstruction {
    int xn;
} RegisterBranchInstruction;

typedef struct ConditionalBranchInstruction {
    int simm19;
} ConditionalBranchInstruction;

// Data Transfer Instruction

typedef enum AddressingMode {
    ADDR_UNSIGNED_OFFSET,
    ADDR_PRE_INDEXED,
    ADDR_POST_INDEXED,
    ADDR_REGISTER_OFFSET
} AddressingMode;

typedef struct SingleDataTransferInstruction {
    bool sf;
    bool L;
    AddressingMode mode;
    int offset;
    int xm;
    int xn;
    int rt;
} SingleDataTransferInstruction;

typedef struct LoadLiteralInstruction {
    bool sf;
    int simm19;
    int rt;
} LoadLiteralInstruction;

// Directive
typedef struct DirectiveInt {
    int value;
} DirectiveInt;

// Main Instruction Struct
typedef struct Instruction {
    OpType op_type;
    union InstructionData {
        ImmediateArithmeticInstruction immediate_arithmetic;
        WideMoveInstruction wide_move;
        RegisterMultiplyInstruction multiply;
        RegisterArithmeticLogicInstruction register_arithmetic_logic;
        UnconditionalBranchInstruction uncond_branch;
        RegisterBranchInstruction reg_branch;
        ConditionalBranchInstruction cond_branch;
        SingleDataTransferInstruction single_data_transfer;
        LoadLiteralInstruction load_literal;
        DirectiveInt directive_int;
    } data;
} Instruction;

#endif
