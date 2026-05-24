#include "memory.h"
#include <stdlib.h>
#include <stdio.h>
#define MEM_SIZE (1 << 20)

/*
Memory is stored as little-endian. MSB is at the highest memory address, LSB is at the lowest memory address.
It expects data to contain the MSB first such that.

THIS CODE ONLY WORKS ON LITTLE ENDIAN machines ie. x86 and ARM.
*/

void check_mem_addr(Memory m, long addr, long n)
{
    if (addr < 0 || n <= 0 || addr + n >= MEM_SIZE)
    {
        // Address and size combination invalid
        // todo handle error
    }
}

/*
For both write and read:

We store the LSB at the lowest address. We reuquire this code to be run on a little endian machine.
On a little endian machine, the LSB is stored at the lowest address so our pointer arithmetic works. On a big endian machine, the logic would break as we would then store the MSB at the lowest address, meaning our emulation fails.
*/

void write(Memory m, long addr, char *data, long n)
{
    check_mem_addr(m, addr, n);
    for (int i = 0; i < n; i++)
    {
        *(m + addr + i) = data[i];
    }
}

void read(Memory m, long addr, char *data, long n)
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
