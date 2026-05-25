#include "common/util.h"
#include "common/hashset.h"

typedef struct {
    char* data;
    AddressSet *accessed;
} MemoryState;

typedef MemoryState* Memory;

/*
For both reading and writing, data must be in little-endian format, ie. the LSB is at index 0.
*/

/*
Write the first n bytes of data to memory starting from base addr
*/
void write(Memory m, uint64 addr, char *data, long n);
/*
Read the first n bytes in memory starting from base addr and write these to data.
Pre: sizeof(*data) = n
*/
void read(Memory m, uint64 addr, char *data, long n);

/*
Returns an array of accessed memory addresses, and fills in `out_size` with the count
The caller is responsible for freeing the returned array.
*/
char** get_accessed_memory(Memory m, int *out_size);

Memory init_mem();
void destroy_mem(Memory m);
