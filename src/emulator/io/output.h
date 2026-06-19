#ifndef OUTPUT_H
#define OUTPUT_H

#include "emulator/state/state.h"

extern void fwrite_all(State *state, const char *path);
extern void print_all(State *state);

#endif
