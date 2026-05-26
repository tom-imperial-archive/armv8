#include "decode.h"
#include "util.h"

// TODO: check if simm and imm need to be handled differently when extracting them.

DecodeResult decode(uint32* input, int input_size, Instruction* result) {
    for (int i = 0; i < input_size; i++) {
        uint32 value = input[i];
        Instruction instruction;

        if (value == 0x8A000000) { // HALT
            OpType op_type = OP_TYPE_HALT;
            instruction = (Instruction) {
                .op_type = op_type
            };
        } else if (value & 0x1C000000 == 0x10000000) { // Data Processing Instruction (Immediate)
            bool sf  = value & 0x80000000 == 0x80000000;
            int opc = (value & 0x60000000) >> 28;
            int opi = (value & 0x03800000) >> 22;
            int operand = value & 0x007FFFF0;
            int rd = value & 0x0000000F;

            if (opi == 0b0010) { // Immediate Arithmetic
                OpType op_type;
                switch (opc) {
                    case 0b00: op_type =  OP_TYPE_ADD; break;
                    case 0b01: op_type = OP_TYPE_ADDS; break;
                    case 0b10: op_type = OP_TYPE_SUB; break;
                    case 0b11: op_type = OP_TYPE_SUBS; break;
                }
                const int MASK_SH = 0x00400000;
                const int MASK_IMM12 = 0x003FFC00;
                const int MASK_RN = 0x000003E0;
                bool rn = (operand & MASK_RN) >> 5;
                int sh = (operand & MASK_SH) >> 22;
                int imm12 = (operand & MASK_IMM12) >> 10;

                instruction = (Instruction) {
                    .op_type = op_type,
                    .data.immediate_arithmetic = {
                        .imm12 = imm12,
                        .rd = rd,
                        .rn = rn,
                        .sf = sf,
                        .sh = sh,
                    }
                };
            } else if (opi == 0b0101) { // Immediate Wide Move
                OpType op_type;
                switch (opc) {
                    case 0b00: op_type =  OP_TYPE_MOVN; break;
                    case 0b01: return DECODE_UNDEFINED_OPCODE;
                    case 0b10: op_type = OP_TYPE_MOVZ; break;
                    case 0b11: op_type = OP_TYPE_MOVK; break;
                }
                const int MASK_HW = 0x00600000;
                const int MASK_IMM16 = 0x001FFFFE;
                int hw = (operand & MASK_HW) >> 21;
                int imm16 = (operand & MASK_IMM16) >> 5;

                // TODO: do I shift imm16 by hw straight away or should this be in EXECUTE
                instruction = (Instruction) {
                    .op_type = op_type,
                    .data.wide_move = {
                        .hw = hw,
                        .imm16 = imm16,
                        .rd = rd,
                        .sf = sf,
                    }
                };
            } else {
                return DECODE_UNDEFINED_OPCODE;
            }
        } else if (input[i] & 0b00001110 == 0b00001010) { // Data Processing Instruction (Register)
            const uint32 MASK_SF = 0x80000000;
            const uint32 MASK_OPC = 0x60000000;
            const uint32 MASK_M = 0x10000000;
            const uint32 MASK_OPR = 0x01E00000;
            const uint32 MASK_RM = 0x001F0000;
            const uint32 MASK_OPERAND = 0x0000FC00;
            const uint32 MASK_RN = 0x000003E0;
            const uint32 MASK_RD = 0x0000001F;
            bool sf = (value & MASK_SF) >> 31;
            int opc = (value & MASK_OPC) >> 29;
            bool m = (value & MASK_M) >> 28;
            int opr = (value & MASK_OPR) >> 21;
            int rm = (value & MASK_RM) >> 16;
            int operand = (value & MASK_OPERAND) >> 10;
            int rn = (value & MASK_RN) >> 5;
            int rd = (value & MASK_RD);

            if (m) { // Multiply
                if ((opr & 0x8) != 0x8) return DECODE_UNDEFINED_OPCODE;
                bool x = operand & 0b100000;
                int ra = operand & 0b011111;
                OpType op_type = x ? OP_TYPE_MSUB : OP_TYPE_MADD;
                instruction = (Instruction) {
                    .op_type = op_type,
                    .data.multiply = {
                        .sf = sf,
                        .ra = ra,
                        .rd = rd,
                        .rm = rm,
                        .rn = rn,
                    }
                };
            } else { // Arithmetic and logic instructions
                int shift_bits = (opr & 0x6) >> 1;
                ShiftType shift_type;
                switch (shift_bits) {
                    case 0b00: shift_type = SHIFT_LSL; break;
                    case 0b01: shift_type = SHIFT_LSR; break;
                    case 0b10: shift_type = SHIFT_ASR; break;
                    case 0b11: shift_type = SHIFT_ROR; break;
                }

                OpType op_type;
                if ((opr & 0x9) == 0x8) { // Arithmetic instruction
                    if ((opr & 0x1) != 0) {
                        return DECODE_UNDEFINED_OPCODE;
                    }
                    switch (opc) {
                        case 0b00: op_type = OP_TYPE_ADD; break;
                        case 0b01: op_type = OP_TYPE_ADDS; break;
                        case 0b10: op_type = OP_TYPE_SUB; break;
                        case 0b11: op_type = OP_TYPE_SUBS; break;
                    }
                    instruction = (Instruction) {
                        .op_type = op_type,
                        .data.register_arithmetic_logic = {
                            .operand = operand,
                            .rd = rd,
                            .rm = rm,
                            .rn = rn,
                            .sf = sf,
                            .shift = shift_type,
                        }
                    };
                } else { // Logic instruction
                    bool n = opr & 0x1;
                    if (n) {
                        switch (opc) {
                            case 0b00: op_type = OP_TYPE_AND; break;
                            case 0b01: op_type = OP_TYPE_ORR; break;
                            case 0b10: op_type = OP_TYPE_EOR; break;
                            case 0b11: op_type = OP_TYPE_ANDS; break;
                        }
                    } else {
                        switch (opc) {
                            case 0b00: op_type = OP_TYPE_BIC; break;
                            case 0b01: op_type = OP_TYPE_ORN; break;
                            case 0b10: op_type = OP_TYPE_EON; break;
                            case 0b11: op_type = OP_TYPE_BICS; break;
                        }
                    }
                }
                instruction = (Instruction) {
                    .op_type = op_type,
                    .data.register_arithmetic_logic = {
                        .operand = operand,
                        .rd = rd,
                        .rm = rm,
                        .rn = rn,
                        .sf = sf,
                        .shift = shift_type,
                    }
                };
            }
        } else if (input[i] & 0b00001010 == 0b00001000) {
            // load/store
            if ((value & 0x80000000) == 0x80000000) {
                // Single Data Transfer: bit 31 = 1
                int sf     = (value & 0x40000000) >> 30;
                int U      = (value & 0x01000000) >> 24;
                int L      = (value & 0x00400000) >> 22;
                int offset = (value & 0x003FFC00) >> 10;
                int xn     = (value & 0x000003E0) >> 5;
                int rt     = (value & 0x0000001F);

                OpType op_type = OP_TYPE_SINGLE_DATA_TRANSFER;

                instruction = (Instruction) {
                    .op_type = op_type,
                    .data.single_data_transfer = {
                        .sf     = sf,
                        .U      = U,
                        .L      = L,
                        .offset = offset,
                        .xn     = xn,
                        .rt     = rt,
                    }
                };

            } else {
                // Load Literal: bit 31 = 0
                int sf     = (value & 0x40000000) >> 30;
                int simm19 = (value & 0x00FFFFE0) >> 5;  // sign-extend after
                int rt     = (value & 0x0000001F);

                OpType op_type = OP_TYPE_LOAD_LITERAL;

                instruction = (Instruction) {
                    .op_type = op_type,
                    .data.load_literal = {
                        .simm19 = simm19,
                        .rt     = rt,
                        .sf     = sf,
                    }
                };
            }

        } else if (input[i] & 0b00011100 == 0b00010100) {
            // branch

            if ((value & 0xFF000000) == 0xD6000000) {
                // Register
                int xn     = (value & 0x000003E0) >> 5;

                OpType op_type = OP_TYPE_AL;

                Instruction instruction = {
                    .op_type = op_type,
                    .data.reg_branch.xn = xn,
                };

            } else if ((value & 0xFF000000) == 0x54000000) {
                // Conditional
                int opcode = (value & 0xFF000000) >> 24;
                int simm19 = (value & 0x00FFFFE0) >> 5;  // sign-extend after
                int cond   = (value & 0x0000000F);

                OpType op_type;

                switch (cond) {
                    case 0b0000: op_type = OP_TYPE_EQ; break;
                    case 0b0001: op_type = OP_TYPE_NE; break;
                    case 0b1010: op_type = OP_TYPE_GE; break;
                    case 0b1011: op_type = OP_TYPE_LT; break;
                    case 0b1100: op_type = OP_TYPE_GT; break;
                    case 0b1101: op_type = OP_TYPE_LE; break;
                    case 0b1110: op_type = OP_TYPE_AL; break;
                }

                instruction = (Instruction) {
                    .op_type = op_type,
                    .data.cond_branch.simm19 = simm19,
                };

            } else if ((value & 0xFC000000) == 0x14000000) {
                // Unconditional
                int opcode = (value & 0xFC000000) >> 26;
                int simm26 = (value & 0x03FFFFFF);

                OpType op_type = OP_TYPE_AL;// sign-extend after

                instruction = (Instruction) {
                    .op_type = op_type,
                    .data.uncond_branch.simm26 = simm26,
                };
            }
        } else {
            return DECODE_UNDEFINED_OPCODE;
        }
        result[i] = instruction;
    }
    return DECODE_SUCCESS;
}
