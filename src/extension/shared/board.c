#include "board.h"
#include <stdlib.h>
#include <stdio.h>
/*
Shared logic for validing ship placements, testing collisions
*/

struct Board {
    ShipDefs ships;
    CellState cells [BOARD_SIZE][BOARD_SIZE];
};

#define check_pos_in_bounds(pos) (pos.x >= 0 && pos.x < BOARD_SIZE && pos.y >= 0 && pos.y < BOARD_SIZE)
#define mask_bit_for_ship(ship) (1 << ship)

static int ship_length(ShipType ship) {
    switch(ship) {
        case SHIP_CARRIER: return 5;
        case SHIP_BATTLESHIP: return 4;
        case SHIP_CRUISER:
        case SHIP_SUBMARINE: return 3;
        case SHIP_DESTROYER: return 2;
        default: return -1;
    }
}

static void next_position(Position *pos) {
    if (pos->horizontal) {
        pos->x++;
    } else {
        pos->y++;
    }
}

static bool place_ship(ShipLocation sl, Board board) {
    int len = ship_length(sl.ship);
    if (len == -1) {
        // Invalid ship type passed
        return false;
    }

    Position pos = sl.pos;

    for (int i = 0; i < len; i++) {
        if (!check_pos_in_bounds(pos)) {
            // Illegal position - the placement must not be valid
            return false;
        }

        if (board->cells[pos.x][pos.y] != CELL_WATER) {
            // Ships overlap
            return false;
        }

        board->cells[pos.x][pos.y] = CELL_SHIP;
        next_position(&pos);
    }

    return true;
}

/*
Add the following ship at given there are already n ships on the board.
It is the caller's responsibility to keep track of how many ships are already on the board.
This should only be used for initialisation and never after gameplay has started.
If you are adding multiple ships, see board_is_valid_placement_set.
*/
bool board_is_valid_placement(ShipLocation sl, Board board, int n) {
    if (n < 0 || n >= NUM_SHIPS) {
        return false;
    }

    bool other_ship_of_same_type = false;

    for (int i = 0; i < n; i++) {
        if (board->ships[i].ship == sl.ship) {
            other_ship_of_same_type = true;
            break;
        }
    }

    if (!other_ship_of_same_type) {
        // No ship of this type declared yet
        if (!place_ship(sl, board)) {
            // Ship position was invalid or ship type invalid
            return false;
        }

        board->ships[n] = sl;
    } else {
        printf("Already ship of same type\n");
        // Tried to add two ships of the same type
        return false;
    }
    return true;
}

/*
Add the following ships at given the board is empty.
It is the caller's responsibility to keep track of how many ships are already on the board.
This should only be used for initialisation and never after gameplay has started.
If you are adding multiple ships, see board_is_valid_placement_set.
*/
bool board_is_valid_placement_set(ShipDefs ship_defs, Board board) {
    // Bit n in declared_types being set to 1 corresponds to having a ship of that type already added
    uint8 declared_types = 0;

    for (int i = 0; i < NUM_SHIPS; i++) {
        ShipLocation sl = ship_defs[i];
        int mask = mask_bit_for_ship(sl.ship);
        if ((declared_types & mask) == 0) {
            // No ship of this type declared yet
            if (!place_ship(sl, board)) {
                // Ship position was invalid or ship type invalid
                return false;
            }

            board->ships[i] = sl;
            declared_types = declared_types | mask;
        } else {
            printf("Already ship of same type\n");
            // Tried to add two ships of the same type
            return false;
        }
    }

    return true;
}

Board create_empty_board(void) {
    Board b = malloc(sizeof(struct Board));
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            b->cells[i][j] = CELL_WATER;
        }
    }

    return b;
}

void free_board(Board b) {
    free(b);
}

/*
Attempts to hit at pos on target_board. If this succeeds, shooter_board has pos
updated to a hit and target_board has pos updated to a hit. If it misses,
the both boards have pos updated to a miss.

Returns true if the shot was successfully processed. Returns false if the position is invalid or
if an attack has already been launched on that position
*/
bool board_try_hit(Board shooter_board, Board target_board, Position pos) {
    if (!check_pos_in_bounds(pos)) {
        return false;
    }

    CellState target = target_board->cells[pos.x][pos.y];

    switch(target) {
        case CELL_WATER: {
            target_board->cells[pos.x][pos.y] = CELL_MISS;
            shooter_board->cells[pos.x][pos.y] = CELL_MISS;
        }; break;
        case CELL_SHIP: {
            target_board->cells[pos.x][pos.y] = CELL_HIT;
            shooter_board->cells[pos.x][pos.y] = CELL_HIT;
        }; break;
        case CELL_HIT:
        case CELL_MISS:
        return false;
    }

    return true;

}

// Testing
int main(void) {
    Board b = create_empty_board();

    ShipLocation defs[5];
    ShipLocation sl0 = {
        .ship = SHIP_CARRIER,
        .pos = {
            .x = 0,
            .y = 0,
            .horizontal = true
        }
    };
    ShipLocation sl1 = {
        .ship = SHIP_BATTLESHIP,
        .pos = {
            .x = 0,
            .y = 1,
            .horizontal = true
        }
    };
    ShipLocation sl2 = {
        .ship = SHIP_CRUISER,
        .pos = {
            .x = 5,
            .y = 0,
            .horizontal = true
        }
    };
    ShipLocation sl3 = {
        .ship = SHIP_SUBMARINE,
        .pos = {
            .x = 0,
            .y = 2,
            .horizontal = false
        }
    };
    ShipLocation sl4 = {
        .ship = SHIP_DESTROYER,
        .pos = {
            .x = 1,
            .y = 2,
            .horizontal = false
        }
    };
    defs[0] = sl0;
    defs[1] = sl1;
    defs[2] = sl2;
    defs[3] = sl3;
    defs[4] = sl4;
    printf("Checking placement: %d\n", board_is_valid_placement_set(defs, b));

    free(b);
    return 0;
}

