#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "shared/protocol.h"
#include "shared/network.h"
#include "server/network.h"
#include "server/game.h"
#include "shared/types.h"
#include "shared/board.h"

struct PlayerState {
    int socket_fd;
    Board board;
};

typedef struct PlayerState *PlayerState;

struct ServerState;
typedef struct ServerState *ServerState;

extern ServerState init_server_state(void);
extern void free_server_state(ServerState state);
extern void setup_boards(ServerState state);

extern bool populate_ships(ServerState state, PlayerState player);
extern PlayerState new_player(int socket_fd);
extern void set_players(ServerState state, PlayerState p1, PlayerState p2);
extern void play(ServerState state);

#endif
