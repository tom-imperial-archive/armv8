#include "memory.h"
#include <stdlib.h>
#include <stdio.h>
#include "common/util.h"
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
        *(m + addr + i) = data[i];
    }
}

void read(Memory m, uint64 addr, char *data, long n)
{
    check_mem_addr(m, addr, n);
    for (int i = 0; i < n; i++)
    {
        data[i] = *(m + addr + i);
    }
}

Memory init_mem()
{
    char *m = malloc(MEM_SIZE);
    return m;
}
void destroy_mem(Memory m)
{
    free(m);
}
