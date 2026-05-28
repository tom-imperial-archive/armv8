#ifndef STATE_H_
#define STATE_H_
#include <stdbool.h>
#include "utils/types.h"
#include "emulator/state/memory.h"
#define REG_PRINT_LINE_LENGTH 24
#define REG_PRINT_BUFFER_SIZE (33 * REG_PRINT_LINE_LENGTH)

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
    N,
    Z,
    C,
    V
};
typedef enum PSTATE_flag PSTATE_flag;

/*
    Represents the registers of an ARMv8 machine and contains a reference to the memory of the machine.
    Do not construct directly; use init_state() instead.
*/
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
    Memory m;
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

void write_reg_64(State *state, Register reg, uint64 val);
void write_reg_32(State *state, Register reg, uint32 val);

uint64 read_reg_64(State *state, Register reg);
uint32 read_reg_32(State *state, Register reg);

void offset_pc(State *state, uint64 offset);
void inc_pc(State *state);
void write_pc(State *state, uint64 value);

void write_pstate_flag(State *state, PSTATE_flag flag, bool val);
bool read_pstate_flag(State *state, PSTATE_flag flag);

void sprint_all_registers(State *state, char *out);
char *sprint_nonzero_memory(State *state);

void write_mem_64(State *state, uint64 addr, uint64 val);
void write_mem_32(State *state, uint64 addr, uint32 val);

uint64 read_mem_64(State *state, uint64 addr);

uint32 read_mem_32(State *state, uint64 addr);
#endif
