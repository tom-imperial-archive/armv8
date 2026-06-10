#ifndef EMULATE_H
#define EMULATE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common/error.h"
#include "common/instruction.h"
#include "emulator/state/state.h"
#include "emulator/decode/decode.h"
#include "emulator/execute/execute.h"
#include "emulator/io/filehandlers.h"
#include "emulator/io/output.h"

// Declare main just for completion
int main(int argc, char **argv);

#endif
