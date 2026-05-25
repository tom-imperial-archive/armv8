#ifndef PARSE_H_

#define PARSE_H_

#include <stdbool.h>

typedef enum DecodeResult {
    DECODE_SUCCESS,
    DECODE_UNDEFINED_OPCODE, // OPI or OPC are not supported in the emulator
    DECODE_UNEXPECTED_END_OF_FILE,
} DecodeResult;

DecodeResult decode(char* input, int input_size, Instruction* result);


typedef enum OpType {
    // Data processing instruction (immediate)

    OP_TYPE_ADD,
    OP_TYPE_ADDS,
    OP_TYPE_SUB,
    OP_TYPE_SUBS,
    // Wide move

    // Move wide with NOT
    OP_TYPE_MOVN,
    // Move wide with zero
    OP_TYPE_MOVZ,
    // Move wide with keep
    OP_TYPE_MOVK,
    // Data processing instruction (register)

    OP_TYPE_AND,
    OP_TYPE_BIC,
    OP_TYPE_ORR,
    OP_TYPE_ORN,
    OP_TYPE_EOR,
    OP_TYPE_EON,
    OP_TYPE_ANDS,
    OP_TYPE_BICS,
    // Bitwise shifts
    OP_TYPE_LSL,
    OP_TYPE_LSR,
    OP_TYPE_ASR,
    OP_TYPE_ROR,
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
} OpType;

typedef struct ArithmeticInstruction {
    int imm12;
    int rd;
    int rn;
    bool sf;
    bool sh;
} ArithmeticInstruction;

typedef struct WideMoveInstruction {
    int hw;
    int imm16;
    bool sf;
    int rd;
} WideMoveInstruction;

typedef struct Instruction {
    OpType op_type;
    union InstructionData {
        ArithmeticInstruction arithmetic;
        WideMoveInstruction wide_move;
        struct DataProcessingInstructionRegister {
            int rd;
            int ra;
            int rn;
            int rm;
        } data_processing_instruction_register;
        struct SingleDataTransfer {
            bool sf;
            bool U;
            bool L;
            int offset;
            int xn;
            int rt;
        } single_data_transfer;
        struct LoadLiteral {
            bool sf;
            int simm19;
            int rt;
        } load_literal;
        struct UnconditionalBranchInstruction {
            int simm26;
        } unconditional_branch;
        struct RegisterBranchInstruction {
            int xn;
        } register_branch;
        struct ConditionalBranchInstruction {
            int simm19;
            int cond;
        } conditional_branch;
    } data;
} Instruction;



#endif
