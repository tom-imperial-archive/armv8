#ifndef PARSE_H_

#define PARSE_H_

#include <stdbool.h>

typedef enum DecodeResult {
    DECODE_SUCCESS,
    DECODE_UNDEFINED_OPCODE, // OPI, OPC, OPR are not supported in the emulator
} DecodeResult;

DecodeResult decode(char* input, int input_size, Instruction* result);


typedef enum OpType {
    // Data processing instruction (immediate). Correspond to `ImmediateArithmeticInstruction`.

    // Halt
    OP_TYPE_HALT,

    // Arithmetic
    OP_TYPE_ADD,
    OP_TYPE_ADDS,
    OP_TYPE_SUB,
    OP_TYPE_SUBS,

    // Wide move
    OP_TYPE_MOVN,
    OP_TYPE_MOVZ,
    OP_TYPE_MOVK,

    // Data processing instruction (register). Correspond to `RegisterArithmeticLogicInstruction`

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

    // Bitwise shifts
    OP_TYPE_LSL,
    OP_TYPE_LSR,
    OP_TYPE_ASR,
    OP_TYPE_ROR,

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
} OpType;

// Data Processing Instruction (Immediate)

//
typedef struct ImmediateArithmeticInstruction {
    int imm12;
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

typedef struct UnconditionalBranchInstruction {
    int simm26;
} UnconditionalBranchInstruction;
typedef struct RegisterBranchInstruction {
    int xn;
} RegisterBranchInstruction;
typedef struct ConditionalBranchInstruction {
    int simm19;
} ConditionalBranchInstruction;

typedef struct SingleDataTransfer {
    bool sf;
    bool U;
    bool L;
    int offset;
    int xn;
    int rt;
} SingleDataTransfer;
typedef struct LoadLiteral {
    bool sf;
    int simm19;
    int rt;
} LoadLiteral;

typedef struct Instruction {
    OpType op_type;
    union InstructionData {
        ImmediateArithmeticInstruction immediate_arithmetic;
        WideMoveInstruction wide_move;
        RegisterMultiplyInstruction multiply;
<<<<<<< HEAD
        RegisterArithmeticLogicInstruction register_arithmetic_logic;
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
=======
        UnconditionalBranchInstruction uncond_branch;
        RegisterBranchInstruction reg_branch;
        ConditionalBranchInstruction cond_branch;
        SingleDataTransfer single_data_transfer;
        LoadLiteral load_literal;
        struct DataProcessingInstructionRegister {
            int rd;
            int ra;
            int rn;
            int rm;
        } data_processing_instruction_register;
        
        
>>>>>>> 5201fa94740f2900a7f3c312fc2d2a216d0c2477
    } data;
} Instruction;

#endif
