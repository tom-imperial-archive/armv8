#include "memory.h"
#include "utils/hashset.h"
#include "utils/types.h"
#include <stdlib.h>
#include <string.h>

#define MEM_SIZE (2 << 20)

/*
Memory is stored in little-endian. MSB is at the highest memory address, LSB is
at the lowest memory address. It expects data to contain the LSB at byte 0.
*/

void check_mem_addr(Memory m, uint64 addr, long n) {
    if (addr < 0 || n <= 0 || addr + n >= MEM_SIZE) {
        // Address and size combination invalid
        // todo handle error
    }
}

void write(Memory m, uint64 addr, uint8 *data, long n) {
    check_mem_addr(m, addr, n);

    // Write data by copying n bytes
    memcpy(&(m->data[addr]), data, n);

    // Track the 4-byte blocks that were altered
    // Calculate 4-byte boundary by ANDing with 11...1100, obtained by negating
    // ..0011
    uint64 start_aligned = addr & ~0x3ULL;
    uint64 end_aligned = (addr + n - 1) & ~0x3ULL;

    // Loop through blocks, 4 bytes at a time
    // This loop runs up to 3 times, depending how many bytes are written, and
    // alignment
    for (uint64 chunk = start_aligned; chunk <= end_aligned; chunk += 4) {
        insert_address(m->accessed, &(m->data[chunk]));
    }
}

void read(Memory m, uint64 addr, uint8 *data, long n) {
    check_mem_addr(m, addr, n);
    // Copy n bytes from the emulated memory to the data buffer
    memcpy(data, &(m->data[addr]), n);
}

NonZeroMemory *get_non_zero_memory(Memory m, int *out_size) {
    if (m == NULL || m->accessed == NULL || m->accessed->size == 0) {
        *out_size = 0;
        return NULL;
    }

    uint8 **accessed_addresses = get_all_addresses(m->accessed);
    NonZeroMemory *results = malloc(m->accessed->size * sizeof(NonZeroMemory));
    int nonzero_count = 0;

    for (int i = 0; i < m->accessed->size; i++) {
        uint8 *physical_address = accessed_addresses[i];

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

Memory init_mem() {
    Memory m = malloc(sizeof(MemoryState));
    m->data = calloc(MEM_SIZE, sizeof(char));
    m->accessed = create_set();
    return m;
}

void destroy_mem(Memory m) {
    if (m == NULL)
        return;
    free_set(m->accessed);
    free(m->data);
    free(m);
}
