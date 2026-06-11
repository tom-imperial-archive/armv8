#ifndef CLIENT_STATE_H
#define CLIENT_STATE_H

#include "logic.h"
#include "shared/board.h"
#include <stdbool.h>

typedef struct {
    ClientUIState ui_state;

    Board own_board;
    Board enemy_board;

    bool is_my_turn;
    bool i_won;
} ClientState;

#endif
