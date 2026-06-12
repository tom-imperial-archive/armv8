#include "state.h"
#include "shared/board.h"

/*
Stores this player's board, opponents board, and current UI phase
*/


ClientState init_client_state(void) {
    ClientState state;

    state.connection_fd = -1;
    state.current_state = UI_STATE_CONNECTING;
    state.is_running = false;
    state.i_won = false;

    state.my_board = create_empty_board();
    state.target_board = create_empty_board();

    state.ships_placed = 0;
    state.placing_horizontal = true;

    for (int i = 0; i < NUM_SHIPS; i++) {
        state.my_placements[i].ship = 0;
        state.my_placements[i].pos.x = 0;
        state.my_placements[i].pos.y = 0;
        state.my_placements[i].pos.horizontal = true;
    }

    return state;
}

void free_client_state(ClientState *state) {
    if (state->my_board != NULL) {
        free_board(state->my_board);
        state->my_board = NULL;
    }

    if (state->target_board != NULL) {
        free_board(state->target_board);
        state->target_board = NULL;
    }
}




