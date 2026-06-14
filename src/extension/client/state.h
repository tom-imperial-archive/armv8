#ifndef CLIENT_STATE_H
#define CLIENT_STATE_H

#include <stdbool.h>
#include "shared/board.h"

typedef enum {
    UI_STATE_CONNECTING,
    UI_STATE_PLACING_SHIPS,
    UI_STATE_WAITING_FOR_OPPONENT,
    UI_STATE_MY_TURN,
    UI_STATE_OPPONENT_TURN,
    UI_STATE_GAME_OVER
} UIState;

typedef struct {
    // Network and status
    int connection_fd;
    UIState current_state;
    bool is_running; // while loop condition
    bool i_won;

    // Local data
    Board my_board;
    Board target_board;

    // Temporary data when placing, irrelevant once main loop has begun
    InitialShipDefs my_placements;
    int ships_placed;
    bool placing_horizontal;
    bool server_requested_board;
} ClientState;

extern ClientState init_client_state(void);
extern void free_client_state(ClientState *state);

#endif
