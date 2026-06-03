#ifndef OUTPUT_H
#define output_h

#include "emulator/state/state.h"

void fwrite_all(State *state, const char *path);
void print_all(State *state);

#endif
