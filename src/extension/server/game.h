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

struct GameState;
typedef struct GameState *GameState;

extern GameState init_game_state(void);
extern void free_game_state(GameState state);
extern void close_connections(GameState state);
extern void setup_boards(GameState state);

extern bool populate_ships(GameState state, PlayerState player);
extern PlayerState new_player(int socket_fd);
extern void set_players(GameState state, PlayerState p1, PlayerState p2);
extern void play(GameState state);

#endif
