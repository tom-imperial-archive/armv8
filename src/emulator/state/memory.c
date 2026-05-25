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
        uint64 current_offset = addr + i;

        // Calculate 4-byte boundary by ANDing with 11...1100, obtained by negating ..0011
        uint64 aligned_offset = current_offset & ~0x3ULL;

        char *aligned_address = &(m->data[aligned_offset]);
        insert_address(m->accessed, aligned_address);

        m->data[current_offset] = data[i];
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

NonZeroMemory* get_non_zero_memory(Memory m, int *out_size)
{
    if (m == NULL || m->accessed == NULL || m->accessed->size == 0) {
        *out_size = 0;
        return NULL;
    }

    char **accessed_addresses = get_all_addresses(m->accessed);
    NonZeroMemory *results = malloc(m->accessed->size * sizeof(NonZeroMemory));
    int nonzero_count = 0;

    for (int i = 0; i < m->accessed->size; i++) {
        char *physical_address = accessed_addresses[i];

        // Read all 4 bytes at once
        uint32_t value = *(uint32_t *)physical_address;

        // Check non-zero
        if (value != 0) {
            // Calculate emulated address via pointer arithmetic
            uint64 emulated_address = (uint64)(physical_address - m->data);

            results[nonzero_count].address = emulated_address;
            results[nonzero_count].value = value;
            nonzero_count++;
        }
    }

    // If every modified address was zero
    free(accessed_addresses);
    if (nonzero_count == 0) {
        free(results);
        *out_size = 0;
        return NULL;
    }

    *out_size = nonzero_count;
    return results;

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
