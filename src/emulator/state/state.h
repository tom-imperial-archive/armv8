#include <stdbool.h>
#include "common/util.h"
/*
This includes all registers including the special registers, excluding PSTATE which is handled separately.
Instructions cannot modify PSTATE so we implement the fields separately.

These values represent PHYSICAL registers.
*/
enum Register
{
    R0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,
    R16,
    R17,
    R18,
    R19,
    R20,
    R21,
    R22,
    R23,
    R24,
    R25,
    R26,
    R27,
    R28,
    R29,
    R30,
    ZR,
    PC,
    SP
};
typedef enum Register Register;

/*
Represents the fields of PSTATE
*/
enum PSTATE_flag
{
    Z,
    N,
    C,
    V
};
typedef enum PSTATE_flag PSTATE_flag;

struct State
{
    uint64 R0;
    uint64 R1;
    uint64 R2;
    uint64 R3;
    uint64 R4;
    uint64 R5;
    uint64 R6;
    uint64 R7;
    uint64 R8;
    uint64 R9;
    uint64 R10;
    uint64 R11;
    uint64 R12;
    uint64 R13;
    uint64 R14;
    uint64 R15;
    uint64 R16;
    uint64 R17;
    uint64 R18;
    uint64 R19;
    uint64 R20;
    uint64 R21;
    uint64 R22;
    uint64 R23;
    uint64 R24;
    uint64 R25;
    uint64 R26;
    uint64 R27;
    uint64 R28;
    uint64 R29;
    uint64 R30;
    uint64 ZR;
    uint64 PC;
    uint64 SP;
    bool N;
    bool Z;
    bool C;
    bool V;
};
typedef struct State State;

/*
Returns a pointer to a correctly initialised State.
*/
State *init_state();
/*
Frees the memory attached to state
*/
void destroy_state(State *state);

/*
For all the following functions, it is the responsibility of the CALLER to ensure that they call the correct 64-bit or 32-bit function.
Only unsigned int types are used in C, which represent the raw bits in the register, even if the actual number saved is stored in 2s-complement.
If the register stores signed data, this conversion must be done elsewhere.
*/

/*
PC and ZR cannot be modified by write functions.
*/
void write_reg_64(State *state, Register reg, uint64 val);
void write_reg_32(State *state, Register reg, uint32 val);

uint64 read_reg_64(State *state, Register reg);
uint32 read_reg_32(State *state, Register reg);

void offset_pc(State *state, uint64 offset);
void inc_pc(State *state);

/*
Debug
*/

void print_all_registers(State *state, char* strbuf);
