#include "state.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define PRINT_LINE_LENGTH 25
#define MASK_LOWER_32BITS (uint32) 0xFFFFFFFFUL

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
        error(STATE_REG_NOT_EXISTS);
    }
}

void check_writeable_register(Register reg)
{
    check_register(reg);
    if (reg == ZR || reg == PC)
    {
        printf("%d", reg);
        error(STATE_WRITE_NOT_ALLOWED);
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
        error(STATE_READ_32_FROM_PC);
    }

    return read_reg_64(state, reg) & MASK_LOWER_32BITS;
}

void destroy_state(State *state)
{
    destroy_mem(state->m);
    free(state);
}

void offset_pc(State *state, uint64 offset)
{
    state->PC += offset;
}

void inc_pc(State *state)
{
    state->PC += 4;
}

bool *get_pstate_flag(State *state, PSTATE_flag pstate)
{
    return &(state->N) + pstate;
}

void check_valid_flag(State *state, PSTATE_flag pstate) {
    if (pstate < 0 || pstate > V)
    {
        error(STATE_FLAG_NOT_EXISTS);
    }
}

void write_pstate_flag(State *state, PSTATE_flag flag, bool val)
{
    check_valid_flag(state, flag);
    *get_pstate_flag(state, flag) = val;
}

bool read_pstate_flag(State *state, PSTATE_flag flag)
{
    check_valid_flag(state, flag);
    return *get_pstate_flag(state, flag);
}

void write_mem_64(State *state, uint64 addr, uint64 val) {
    write(state->m, addr, (uint8 *)&val, sizeof(uint64));
}

void write_mem_32(State *state, uint64 addr, uint32 val) {
    write(state->m, addr, (uint8 *)&val, sizeof(uint32));
}

uint64 read_mem_64(State *state, uint64 addr) {
    uint64 val = 0;
    read(state->m, addr, (uint8 *)&val, sizeof(uint64));
    return val;
}

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

void sprint_all_registers(State *state, char *out)
{
    // Assume 25 chars per line. 38 lines so 950 chars of space required
    char nextLine[PRINT_LINE_LENGTH];

    // General purpose
    for (int r = R0; r <= R30; r++)
    {
        uint64 val = read_reg_64(state, r);
        sprintf(nextLine, "X%.2d = %lx\n", r, val);
        strcat(out, nextLine);
    }

    // Special
    sprintf(nextLine, "ZR = %lx\n", read_reg_64(state, ZR));
    strcat(out, nextLine);

    sprintf(nextLine, "PC = %lx\n", read_reg_64(state, PC));
    strcat(out, nextLine);

    sprintf(nextLine, "SP = %lx\n", read_reg_64(state, SP));
    strcat(out, nextLine);

    //PSTATE
    sprintf(nextLine, "PSTATE : %c", pstate_flag_to_char(N, state));
    strcat(out, nextLine);
    sprintf(nextLine, "%c", pstate_flag_to_char(Z, state));
    strcat(out, nextLine);
    sprintf(nextLine, "%c", pstate_flag_to_char(C, state));
    strcat(out, nextLine);
    sprintf(nextLine, "%c\n", pstate_flag_to_char(V, state));
    strcat(out, nextLine);
}

void sprint_nonzero_memory(State *state, char *out)
{
    // As before, 25 chars per line.
    char new[25];
    int nonzero_count = 0;

    NonZeroMemory *memory_data = get_non_zero_memory(state->m, &nonzero_count);

    if (memory_data == NULL) {
        return;
    }

    sprintf(new, "Non-zero memory:\n");
    strcat(out, new);

    for (int i = 0; i < nonzero_count; i++) {
        // 0x%08lx formats the 64-bit address has an 8-character zero-padded hex value
        // 0x%08x formats the 32-bit chunk of data as an 8-character zero-added hex value
        sprintf(new, "0x%08lx: 0x%08x\n", memory_data[i].address, memory_data[i].value);
        strcat(out, new);
    }
    free(memory_data);
}
