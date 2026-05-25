#include "memory.h"
#include <stdlib.h>
#include <stdio.h>
#include "common/util.h"
#include "common/hashset.h"
#define MEM_SIZE (2 << 20)

/*
Memory is stored in little-endian. MSB is at the highest memory address, LSB is at the lowest memory address.
It expects data to contain the LSB at byte 0.
*/

void check_mem_addr(Memory m, uint64 addr, long n)
{
    if (addr < 0 || n <= 0 || addr + n >= MEM_SIZE)
    {
        // Address and size combination invalid
        // todo handle error
    }
}

void write(Memory m, uint64 addr, char *data, long n)
{
    check_mem_addr(m, addr, n);
    for (int i = 0; i < n; i++)
    {
        char *address = &(m->data[addr + i]);
        insert_address(m->accessed, address);
        *address = data[i];
    }
}

void read(Memory m, uint64 addr, char *data, long n)
{
    check_mem_addr(m, addr, n);
    for (int i = 0; i < n; i++)
    {
        data[i] = m->data[addr + i];
    }
}

char** get_accessed_memory(Memory m, int *out_size) {
    if (m == NULL || m->accessed == NULL || m->accessed->size == 0) {
        *out_size = 0;
        return NULL;
    }

    *out_size = m->accessed->size;

    return get_all_addresses(m->accessed);
}

Memory init_mem()
{
    Memory m = malloc(sizeof(MemoryState));
    m->data = calloc(MEM_SIZE, sizeof(char));
    m->accessed = create_set();
    return m;
}



void destroy_mem(Memory m)
{
    if (m == NULL) return;
    free_set(m->accessed);
    free(m->data);
    free(m);
}
