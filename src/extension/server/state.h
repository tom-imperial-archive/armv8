#ifndef SERVER_STATE_H
#define SERVER_STATE_H

#include "shared/types.h"
#include "shared/board.h"
#include <stdbool.h>

typedef enum {
    GAME_WAITING_FOR_CONNECTIONS,
    GAME_PLACING_SHIPS,
    GAME_P1_TURN,
    GAME_P2_TURN,
    GAME_OVER
} GameState;

typedef struct {
    int socket_fd;
    Board real_board;
    bool is_ready;
} PlayerState;

typedef struct {
    GameState current_state;
    PlayerState player1;
    PlayerState player2;
} ServerState;

#endif
