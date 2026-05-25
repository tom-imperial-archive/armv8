#include "decode.h"
#include "util.h"


DecodeResult decode(char* input, int input_size, Instruction* result) {
    // Instruction* result = malloc(sizeof(Instruction) * input_size);
    for (int i = 0; i < input_size; i += 4) {
        uint32 value = ((uint32)input[i    ] << 24) | ((uint32)input[i + 1] << 16)
                     | ((uint32)input[i + 2] << 8 ) | ((uint32)input[i + 3]);
        if (value & 0x1C000000 == 0x10000000) { // is dp_imm
            bool sf  = value & 0x80000000 == 0x80000000;
            int opc = (value & 0x60000000) >> 28; // TODO: check if shift is correct
            int opi = (value & 0x03800000) >> 22;
            int operand = value & 0x007FFFF0;
            int rd = value & 0x0000000F;

            if (opi == 0b0010) { // arithmetic
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

                Instruction instruction = {
                    .op_type = op_type,
                    .data.arithmetic = {
                        .imm12 = imm12,
                        .rd = rd,
                        .rn = rn,
                        .sf = sf,
                        .sh = sh,
                    }
                };
                // TODO: add instruction to result
            } else if (opi == 0b0101) { // wide move
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
                Instruction instruction = {
                    .op_type = op_type,
                    .data.wide_move = {
                        .hw = hw,
                        .imm16 = imm16,
                        .rd = rd,
                        .sf = sf,
                    }
                };
                // TODO: add instruction to result
            } else { // error: undefined opi
                return DECODE_UNDEFINED_OPCODE;
            }
        } else if (input[i] & 0b00001110 == 0b00001010) { // dp_reg

        } else if (input[i] & 0b00001010 == 0b00001000) { // load/store

        } else if (input[i] & 0b00011100 == 0b00010100) { // branch

        } else { // error

        }
    }


    return DECODE_SUCCESS;
}
