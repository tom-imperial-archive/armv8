#ifndef MEMORY_H
#define MEMORY_H

#include "utils/hashset.h"
#include "utils/types.h"

typedef struct {
    uint8 *data;
    AddressSet *accessed;
} MemoryState;

typedef MemoryState *Memory;

typedef struct {
    uint64 address;
    uint32 value;
} NonZeroMemory;

/*
For both reading and writing, data must be in little-endian format, ie. the LSB
is at index 0.
*/

/*
Write the first n bytes of data to memory starting from base addr
*/
extern void write(Memory m, uint64 addr, uint8 *data, long n);
/*
Read the first n bytes in memory starting from base addr and write these to
data. Pre: sizeof(*data) = n
*/
extern void read(Memory m, uint64 addr, uint8 *data, long n);

/*
Returns the 4 byte chunks of memory that are non-zero, and their values
and fills in `out_size` with the count.
The caller is responsible for freeing the returned array.
*/
extern NonZeroMemory *get_non_zero_memory(Memory m, int *out_size);

extern Memory init_mem();
extern void destroy_mem(Memory m);

#endif
