#ifndef ARMV8_58_READFILE_H
#define ARMV8_58_READFILE_H

#include "utils/types.h"
#include <stdbool.h>

uint32 *readfile(const char *path, size_t *count);
bool writefile(const char *path, char *registers_out, char *nonzero_out);

#endif //ARMV8_58_READFILE_H
