#include "board.h"

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

static bool place_ship(ShipState sl, Board board) {
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
bool board_add_placement(ShipState sl, Board board, int n) {
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
bool board_add_placement_set(ShipDefs ship_defs, Board board) {
    // Bit n in declared_types being set to 1 corresponds to having a ship of that type already added
    uint8 declared_types = 0;

    for (int i = 0; i < NUM_SHIPS; i++) {
        ShipState sl = ship_defs[i];
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

static void check_destroyed(Board board, ShipState *s) {
    int len = ship_length(s->ship);
    Position pos = s->pos;
    s->destroyed = true;
    for (int i = 0; i < len; i++) {
        if (board->cells[pos.x][pos.y] != CELL_HIT) {
            s->destroyed = false;
            break;
        }
        next_position(&pos);
    }
}

static void for_each_ship(Board board, void (*cb)(Board board, ShipState *s)) {
    for (int i = 0; i < NUM_SHIPS; i++) {
        (*cb)(board, &(board->ships[i]));
    }
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

            // Update the relevant ship definitions on the target board
            for_each_ship(target_board, *check_destroyed);
        }; break;
        case CELL_HIT:
        case CELL_MISS:
        return false;
    }

    return true;
}

bool all_ships_destroyed(Board board) {
    for (int i = 0; i < NUM_SHIPS; i++) {
        if (!board->ships[i].destroyed) {
            return false;
        }
    }

    return true;
}

static char cell_to_char(CellState cell) {
    switch(cell) {
        case CELL_HIT: return 'X'; break;
        case CELL_MISS: return 'O'; break;
        case CELL_SHIP: return '*'; break;
        default: return '-';
    }
}

void print_board(Board board, FILE *out) {
    fprintf(out, " ");
    for (int i = 0; i < BOARD_SIZE; i++) {
        fprintf(out, " %d", i);
    }
    fprintf(out, "\n");

    for(int i = 0; i < BOARD_SIZE; i++) {
        fprintf(out, "%d", i);
        for(int j = 0; j < BOARD_SIZE; j++) {
            fprintf(out, " %c", cell_to_char(board->cells[j][i]));
        }
        fprintf(out, "\n");
    }
}

// Testing
/*
int main(void) {
    Board b = create_empty_board();

    ShipState defs[5];
    ShipState sl0 = {
        .ship = SHIP_CARRIER,
        .pos = {
            .x = 0,
            .y = 0,
            .horizontal = true
        }
    };
    ShipState sl1 = {
        .ship = SHIP_BATTLESHIP,
        .pos = {
            .x = 0,
            .y = 1,
            .horizontal = true
        }
    };
    ShipState sl2 = {
        .ship = SHIP_CRUISER,
        .pos = {
            .x = 5,
            .y = 0,
            .horizontal = true
        }
    };
    ShipState sl3 = {
        .ship = SHIP_SUBMARINE,
        .pos = {
            .x = 0,
            .y = 2,
            .horizontal = false
        }
    };
    ShipState sl4 = {
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
    printf("Checking placement: %d\n", board_add_placement_set(defs, b));
    print_board(b, stdout);

    Board source_board = create_empty_board();
    Position p = {
            .x = 1,
            .y = 2,
        };
    printf("%d\n", board_try_hit(source_board, b, p));
    p.x = 5;
    printf("%d\n", board_try_hit(source_board, b, p));

    printf("Source: \n");
    print_board(source_board, stdout);
    printf("Board: \n");
    print_board(b, stdout);

    printf("Game over? %d\n", all_ships_destroyed(b));

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            p.x = i;
            p.y = j;
            board_try_hit(source_board, b, p);
        }
    }

    printf("Source: \n");
    print_board(source_board, stdout);
    printf("Board: \n");
    print_board(b, stdout);
    printf("Game over? %d\n", all_ships_destroyed(b));

    free(source_board);
    free(b);
    return 0;
}
*/
