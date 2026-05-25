#include "common/util.h"

typedef char* Memory;

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

Memory init_mem();
void destroy_mem(Memory m);
