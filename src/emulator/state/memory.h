#include "common/util.h"
/*
Write the first n bytes of data to memory starting from base addr
*/
void write(long addr, char *data, long n);
/*
Read the first n bytes in memory starting from base addr and write these to data.
*/
void read(long addr, char *data, long n);