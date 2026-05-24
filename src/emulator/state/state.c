#include <stdlib.h>
#include "state.h"
#include <stdio.h>

State *init_state()
{
    State *state = malloc(sizeof(State));

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
    return (bool *)state + pstate;
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

bool read_pstate_flag(State *state, PSTATE_flag pstate)
{
    if (pstate < 0 || pstate > V)
    {
        // Invalid register. Handle error
    }

    return *get_pstate_flag(state, pstate);
}

void print_all_registers(State *state)
{
    for (int r = 0; r <= SP; r++)
    {
        uint64 val = read_reg_64(state, r);
        printf("R%d: %ld\n", r, val);
    }
    for (int p = 0; p <= V; p++)
    {
        bool val = read_pstate_flag(state, p);
        printf("P%d: %d\n", p, val);
    }
}