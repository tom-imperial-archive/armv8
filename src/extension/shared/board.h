#ifndef BOARD_H
#define BOARD_H

#include "types.h"
#include <stdbool.h>

typedef enum {
    CELL_WATER,
    CELL_SHIP,
    CELL_MISS,
    CELL_HIT
} CellState;

// 0, 0 is the top left
struct Board;
typedef struct Board *Board;

typedef struct {
    int x;
    int y;
    bool horizontal;
} Position;

typedef struct {
    ShipType ship;
    Position pos;
} ShipLocation;

typedef ShipLocation ShipDefs[NUM_SHIPS];

/*
Returns true if the ship placements form a valid board. If it is a valid placement, populate the board
*/
extern bool board_is_valid_placement_set(ShipDefs ship_defs, Board board);
extern bool board_is_valid_placement(ShipLocation sl, Board board, int nships);

extern Board create_empty_board(void);

#endif
