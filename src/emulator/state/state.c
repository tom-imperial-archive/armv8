#include "state.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "utils/bitmasks.h"
#define INSTRUCTION_LENGTH 4

typedef enum {STATE_REG_NOT_EXISTS, STATE_WRITE_NOT_ALLOWED, STATE_READ_32_FROM_PC, STATE_FLAG_NOT_EXISTS} StateError;

//todo refactor errors accross the project
void error(StateError error) {
    switch(error) {
        case STATE_REG_NOT_EXISTS: printf("Register does not exist"); break;
        case STATE_WRITE_NOT_ALLOWED: printf("Illegal write to register"); break;
        case STATE_READ_32_FROM_PC: printf("Illegal 32-bit read from PC"); break;
        case STATE_FLAG_NOT_EXISTS: printf("PSTATE flag does not exist"); break;
        default: printf("Unknown error accessing registers");
    }
    exit(1);
}


static uint64 *get_register(State *state, Register reg)
{
    return (uint64 *)state + reg;
}

static void clear_reg(State *state, Register reg) {
    *get_register(state, reg) = 0;
}

/*
    Creates a State representing the cleared, initial state of an ARMv8 machine.
    Returns the pointer to this state.
*/
State *init_state()
{
    State *state = malloc(sizeof(State));

    state->m = init_mem();
    // Clear all registers except PSTATE
    for (int r = 0; r <= SP; r++)
    {
        clear_reg(state, r);
    }

    // Set default values of PSTATE
    state->N = false;
    state->Z = true;
    state->C = false;
    state->V = false;

    return state;
}

static void check_register(Register reg)
{
    if ((reg < R0) || (reg > SP))
    {
        error(STATE_REG_NOT_EXISTS);
    }
}

static void check_writeable_register(Register reg)
{
    check_register(reg);
    if (reg == PC)
    {
        error(STATE_WRITE_NOT_ALLOWED);
    }
}

/*
    Sets the contents of the 64-bit register reg to val
    Writes to ZR are ignored. Writes directly to the PC throw an error: use an appropriate PC modification function.
*/
void write_reg_64(State *state, Register reg, uint64 val)
{
    check_writeable_register(reg);
    if (reg != ZR) {
        uint64 *rp = get_register(state, reg);
        *rp = val;
    }
}

/*
    Sets the lower 32-bits of the register reg to val. Clears the upper 32-bits.
    Writes to WZR are ignored. Writes directly to the PC throw an error: use an appropriate PC modification function.
*/
void write_reg_32(State *state, Register reg, uint32 val)
{
    write_reg_64(state, reg, (uint64)val);
}

/*
    Returns the contents of the 64-bit register reg
*/
uint64 read_reg_64(State *state, Register reg)
{
    check_register(reg);
    return *get_register(state, reg);
}

/*
    Returns the lower 32-bits of the register reg
*/
uint32 read_reg_32(State *state, Register reg)
{
    if (reg == PC)
    {
        error(STATE_READ_32_FROM_PC);
    }

    return read_reg_64(state, reg) & BITMASK_LOWER_32_BITS;
}

/*
    Frees all memory associated with the state
*/
void destroy_state(State *state)
{
    destroy_mem(state->m);
    free(state);
}

/*
    Adds the value of offset to the PC. To subtract offset from the PC, use the 2's complement of offset
*/
void offset_pc(State *state, uint64 offset)
{
    state->PC += offset;
}

/*
    Sets the PC to value
*/
void inc_pc(State *state)
{
    state->PC += INSTRUCTION_LENGTH;
}

/*
    Sets the PC to value
*/
void write_pc(State *state, uint64 value)
{
    state->PC = value;
}

static void check_valid_flag(State *state, PSTATE_flag flag) {
    assert(flag >= 0 && flag <= V);
}

static bool *get_pstate_flag(State *state, PSTATE_flag flag)
{
    check_valid_flag(state, flag);
    switch(flag) {
        case N: return &state->N;
        case Z: return &state->Z;
        case C: return &state->C;
        default: return &state->V;
    }
}

/*
    Sets the flag flag in the PSTATE register to val
*/
void write_pstate_flag(State *state, PSTATE_flag flag, bool val)
{
    check_valid_flag(state, flag);
    *get_pstate_flag(state, flag) = val;
}

/*
    Reads the flag flag from the PSTATE register
*/
bool read_pstate_flag(State *state, PSTATE_flag flag)
{
    check_valid_flag(state, flag);
    return *get_pstate_flag(state, flag);
}

/*
    Writes the 32-bit integer val to memory at address addr
*/
void write_mem_64(State *state, uint64 addr, uint64 val) {
    write(state->m, addr, (uint8 *)&val, sizeof(uint64));
}

/*
    Writes the 32-bit integer val to memory at address addr
*/
void write_mem_32(State *state, uint64 addr, uint32 val) {
    write(state->m, addr, (uint8 *)&val, sizeof(uint32));
}

/*
    Returns the unsigned 64-bit integer representing the data at memory address addr
*/
uint64 read_mem_64(State *state, uint64 addr) {
    uint64 val = 0;
    read(state->m, addr, (uint8 *)&val, sizeof(uint64));
    return val;
}

/*
    Returns the unsigned 32-bit integer representing the data at memory address addr
*/
uint32 read_mem_32(State *state, uint64 addr) {
    uint64 val = 0;
    read(state->m, addr, (uint8 *)&val, sizeof(uint32));
    return val;
}

/*
Pre: flag is a valid PSTATE_flag value
*/
char pstate_flag_to_char(PSTATE_flag flag, State *state) {
    if (read_pstate_flag(state, flag)) {
        switch(flag) {
            case N: return 'N';
            case Z: return 'Z';
            case C: return 'C';
            case V: return 'V';
        }
    }
    return '-';
}

/*
    Sprints the content of all registers to the string buffer out.
    Precondition: Size of the out buffer >= PRINT_LINE_LENGTH * 33
*/
void sprint_all_registers(State *state, char *out)
{
    char nextLine[REG_PRINT_LINE_LENGTH];

    sprintf(nextLine, "Registers:\n");
    strcat(out, nextLine);

    // General purpose
    for (int r = R0; r <= R30; r++)
    {
        uint64 val = read_reg_64(state, r);
        sprintf(nextLine, "X%.2d = %016lx\n", r, val);
        strcat(out, nextLine);
    }

    // Special
    sprintf(nextLine, "PC = %016lx\n", read_reg_64(state, PC));
    strcat(out, nextLine);


    //PSTATE
    sprintf(nextLine, "PSTATE : %c", pstate_flag_to_char(N, state));
    strcat(out, nextLine);
    sprintf(nextLine, "%c", pstate_flag_to_char(Z, state));
    strcat(out, nextLine);
    sprintf(nextLine, "%c", pstate_flag_to_char(C, state));
    strcat(out, nextLine);
    sprintf(nextLine, "%c", pstate_flag_to_char(V, state));
    strcat(out, nextLine);
}

/*
    Sprints the content of memory addresses where the value is non-zero.
    Must call free() on the pointer returned when done using.
*/
char *sprint_nonzero_memory(State *state)
{
    int nonzero_count = 0;

    NonZeroMemory *memory_data = get_non_zero_memory(state->m, &nonzero_count);

    char *out = malloc(30 + (25*nonzero_count));
    sprintf(out, "Non-zero memory:\n");

    if (memory_data == NULL) {
        return out;
    }

    char new[REG_PRINT_LINE_LENGTH];
    for (int i = 0; i < nonzero_count; i++) {
        sprintf(new, "0x%08lx: %08x\n", memory_data[i].address, memory_data[i].value);
        strcat(out, new);
    }
    free(memory_data);

    return out;
}

