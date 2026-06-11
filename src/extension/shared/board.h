#ifndef BOARD_H
#define BOARD_H

#include "types.h"

typedef enum {
    CELL_WATER = 0,
    CELL_SHIP = 1,
    CELL_MISS = 2,
    CELL_HIT = 3
} CellState;

typedef uint8 Board[BOARD_SIZE][BOARD_SIZE];

#endif
