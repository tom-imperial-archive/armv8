#include <stdlib.h>
#include "state.h"
#include <stdio.h>
#include <string.h>
#define CAT strcat(out, new);

State *init_state()
{
    State *state = malloc(sizeof(State));

    state->m = init_mem();
    // Clear all registers except PSTATE
    for (int r = 0; r <= SP; r++)
    {
        write_reg_64(state, r, 0);
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
    if (!(reg < R0) || !(reg > SP))
    {
        // todo error handling
    }
}

void check_writeable_register(Register reg)
{
    check_register(reg);
    if (reg == ZR || reg == PC)
    {
        // todo error handling
    }
}

uint64 *get_register(State *state, Register reg)
{
    return (uint64 *)state + reg;
}

void write_reg_64(State *state, Register reg, uint64 val)
{
    check_writeable_register(reg);
    // We have a writable register
    uint64 *rp = get_register(state, reg);
    *rp = val;
}

void write_reg_32(State *state, Register reg, uint32 val)
{
    check_writeable_register(reg);
    // We have a writable register
    uint64 *rp = get_register(state, reg);
    *rp = (uint64)val;
}

uint64 read_reg_64(State *state, Register reg)
{
    check_register(reg);
    return *get_register(state, reg);
}

uint32 read_reg_32(State *state, Register reg)
{
    check_register(reg);

    if (reg == PC)
    {
        // todo error handling
    }

    return (uint32)*get_register(state, reg);
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

void write_pstate_flag(State *state, PSTATE_flag pstate, bool val)
{
    if (pstate < 0 || pstate > V)
    {
        // Invalid register. Handle error
        return;
    }

    bool *flag = get_pstate_flag(state, pstate);
    *flag = val;
}

bool read_pstate_flag(State *state, PSTATE_flag flag)
{
    if (flag < 0 || flag > V)
    {
        // Invalid register. Handle error
    }

    return *get_pstate_flag(state, flag);
}

/*
Pre: flag is a valid PSTATE_flag value
*/
char pstate_flag_to_char(PSTATE_flag flag, State *state) {
    if (read_pstate_flag(state, flag)) {
        printf("Flag %d %d", flag, read_pstate_flag(state, flag));
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
        sprintf(new, "X%d = %ld\n", r, val);
        CAT;
    }

    // Special
    sprintf(new, "ZR = %ld\n", read_reg_64(state, ZR));
    CAT;

    sprintf(new, "PC = %ld\n", read_reg_64(state, PC));
    CAT;

    sprintf(new, "SP = %ld\n", read_reg_64(state, SP));
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
