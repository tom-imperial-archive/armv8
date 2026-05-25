#include "state.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define CAT strcat(out, new);
#define ONES_32BIT (uint32) 0xFFFFFFFFUL

typedef enum {REG_NOT_EXISTS, WRITE_NOT_ALLOWED, READ_32_FROM_PC, FLAG_NOT_EXISTS} StateError;

//todo refactor errors accross the project
void error(StateError error) {
    switch(error) {
        case REG_NOT_EXISTS: printf("Register does not exist"); break;
        case WRITE_NOT_ALLOWED: printf("Illegal write to register"); break;
        case READ_32_FROM_PC: printf("Illegal 32-bit read from PC"); break;
        case FLAG_NOT_EXISTS: printf("PSTATE flag does not exist"); break;
        default: printf("Unknown error accessing registers");
    }
    exit(1);
}


uint64 *get_register(State *state, Register reg)
{
    return (uint64 *)state + reg;
}

void clear_reg(State *state, Register reg) {
    *get_register(state, reg) = 0;
}

State *init_state()
{
    State *state = malloc(sizeof(State));

    state->m = init_mem();
    // Clear all registers except PSTATE
    for (int r = 0; r <= SP; r++)
    {
        clear_reg(state, r);
    }

    // Clear PSTATE
    state->N = false;
    state->Z = false;
    state->C = false;
    state->V = false;

    return state;
}

void check_register(Register reg)
{
    if ((reg < R0) || (reg > SP))
    {
        error(REG_NOT_EXISTS);
    }
}

void check_writeable_register(Register reg)
{
    check_register(reg);
    if (reg == ZR || reg == PC)
    {
        printf("%d", reg);
        error(WRITE_NOT_ALLOWED);
    }
}

void write_reg_64(State *state, Register reg, uint64 val)
{
    check_writeable_register(reg);
    uint64 *rp = get_register(state, reg);
    *rp = val;
}

void write_reg_32(State *state, Register reg, uint32 val)
{
    write_reg_64(state, reg, (uint64)val);
}

uint64 read_reg_64(State *state, Register reg)
{
    check_register(reg);
    return *get_register(state, reg);
}

uint32 read_reg_32(State *state, Register reg)
{
    if (reg == PC)
    {
        error(READ_32_FROM_PC);
    }

    return read_reg_64(state, reg) & ONES_32BIT;
}

void destroy_state(State *state)
{
    destroy_mem(state->m);
    free(state);
}

void offset_pc(State *state, uint64 offset)
{
    state->PC = state->PC + offset;
}

void inc_pc(State *state)
{
    state->PC = state->PC + 4;
}

bool *get_pstate_flag(State *state, PSTATE_flag pstate)
{
    return (bool *) &(state->N) + pstate;
}

void check_valid_flag(State *state, PSTATE_flag pstate) {
    if (pstate < 0 || pstate > V)
    {
        error(FLAG_NOT_EXISTS);
    }
}

void write_pstate_flag(State *state, PSTATE_flag pstate, bool val)
{


    bool *flag = get_pstate_flag(state, pstate);
    *flag = val;
}

bool read_pstate_flag(State *state, PSTATE_flag flag)
{
    check_valid_flag(state, flag);

    return *get_pstate_flag(state, flag);
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

void sprint_all_registers(State *state, char *out)
{
    // Assume 25 chars per line. 38 lines so 950 chars of space required
    char new[25];

    // General purpose
    for (int r = R0; r <= R30; r++)
    {
        uint64 val = read_reg_64(state, r);
        sprintf(new, "X%.2d = %lx\n", r, val);
        CAT;
    }

    // Special
    sprintf(new, "ZR = %lx\n", read_reg_64(state, ZR));
    CAT;

    sprintf(new, "PC = %lx\n", read_reg_64(state, PC));
    CAT;

    sprintf(new, "SP = %lx\n", read_reg_64(state, SP));
    CAT;

    //PSTATE
    sprintf(new, "PSTATE : %c", pstate_flag_to_char(N, state));
    CAT;
    sprintf(new, "%c", pstate_flag_to_char(Z, state));
    CAT;
    sprintf(new, "%c", pstate_flag_to_char(C, state));
    CAT;
    sprintf(new, "%c\n", pstate_flag_to_char(V, state));
    CAT;
}
