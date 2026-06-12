#include "game.h"

/*
Processes actions, overwrites grid cells, and checks for win conditions
*/

struct ServerState {
    PlayerState player1;
    PlayerState player2;
    bool is_player1_turn;
};

/*
Creates an empty game with two null player states
*/
ServerState init_server_state(void) {
    ServerState state = malloc(sizeof(struct ServerState));
    if (state == NULL) {
        return NULL;
    }

    state->player1 = NULL;
    state->player2 = NULL;
    state->is_player1_turn = rand() > (RAND_MAX / 2);

    srand(time(NULL));

    return state;
}

PlayerState new_player(int socket_fd) {
    PlayerState p = malloc(sizeof(struct PlayerState));
    if (p == NULL) {
        return NULL;
    }

    p->socket_fd = socket_fd;
    p->board = create_empty_board();
    return p;
}

void set_players(ServerState state, PlayerState p1, PlayerState p2) {
    state->player1 = p1;
    state->player2 = p2;
}

void free_server_state(ServerState state) {
    free(state);
}

void end_game(ServerState state, PlayerState winner, PlayerState loser) {
    GameOverPayload winnerData = {.you_won = true};
    GameOverPayload loserData = {.you_won = false};

    while (send_packet(winner->socket_fd, MSG_GAME_OVER, &winnerData, sizeof(GameOverPayload)) != 0);
    while (send_packet(loser->socket_fd, MSG_GAME_OVER, &loserData, sizeof(GameOverPayload)) != 0);

    close(state->player1->socket_fd);
    close(state->player2->socket_fd);

    free(state->player1->board);
    free(state->player2->board);
    free(state->player1);
    free(state->player2);
    free_server_state(state);
}

/*
Returns true if the ships have valid placements, otherwise return false
*/
bool populate_ships(ServerState state, PlayerState player) {
    PacketHeader header;
    InitialShipDefs *defs = NULL;

    PlayerState other;
    if (player == state->player1) {
        other = state->player2;
    } else {
        other = state->player1;
    }

    while (true) {
        int res = receive_packet(player->socket_fd, &header, (void **) &defs);
        if (res == 1) {
            // Received packet
            if (header.type == MSG_INIT_BOARD_LAYOUT) {
                break;
            }
        } else if (res == -1) {
            //todo failure logic
            end_game(state, other, player);
            exit(EXIT_FAILURE);
        }
    }

    bool res = board_add_placement_set(*defs, player->board);
    free(*defs);
    return res;
}

PlayerState get_player1(ServerState state) {
    return state->player1;
}
PlayerState get_player2(ServerState state) {
    return state->player2;
}

static void send_start_packets(ServerState state) {
    GameStartPayload your_turn = {.your_turn = true};
    GameStartPayload not_your_turn = {.your_turn = false};

    if (state->is_player1_turn) {
        send_packet(state->player1->socket_fd, MSG_GAME_START, &your_turn, sizeof(GameStartPayload));
        send_packet(state->player2->socket_fd, MSG_GAME_START, &not_your_turn, sizeof(GameStartPayload));
    } else {
        send_packet(state->player1->socket_fd, MSG_GAME_START, &not_your_turn, sizeof(GameStartPayload));
        send_packet(state->player2->socket_fd, MSG_GAME_START, &your_turn, sizeof(GameStartPayload));
    }
}

void populate_turn_players(ServerState state, PlayerState *turn_taker, PlayerState *other_player) {
    if (state->is_player1_turn) {
        *turn_taker = state->player1;
        *other_player = state->player2;
    } else {
        *turn_taker = state->player2;
        *other_player = state->player1;
    }
}

void play(ServerState state) {
    // Keep requesting a ship placement until we get a valid one
    printf("Requesting ship positions from the clients");

    // Populate ships
    while (!populate_ships(state, state->player1));
    while (!populate_ships(state, state->player2));

    // Inform clients we are ready and inform them of whos turn it is
    send_start_packets(state);

    PlayerState turn_taker;
    PlayerState other_player;
    // Take turns
    do {
        populate_turn_players(state, &turn_taker, &other_player);

        PacketHeader header;
        FirePayload *fire_payload = NULL;
        int res = receive_packet(turn_taker->socket_fd, &header, (void **) &fire_payload);

        if (res == 1) {
            // Received packet
            if (header.type == MSG_FIRE) {
                Position p = { .x = fire_payload->x, .y = fire_payload->y};

                bool was_hit = false;
                ShipType sunk;

                bool res = board_try_hit(other_player->board, p, &was_hit, &sunk);

                if (!res) {
                    //todo consider if this fails
                } else {
                    // Inform player of result
                    HitResultPayload hrp = {.success = was_hit, .destroyed_ship = sunk};
                    EnemyAttackPayload eap = { .shot = *fire_payload, .result = hrp};

                    send_packet(turn_taker->socket_fd, MSG_ATTACKED, &eap, sizeof(EnemyAttackPayload));
                    // Inform opponent of result
                    send_packet(other_player->socket_fd, MSG_ATTACKED, &eap, sizeof(EnemyAttackPayload));

                    // SWAP TURNS
                    state->is_player1_turn = !state->is_player1_turn;
                }
            }
        } else if (res == -1) {
            //todo failure logic
            end_game(state, other_player, turn_taker);
            exit(EXIT_FAILURE);
        }


    } while (!all_ships_destroyed(other_player->board));

    end_game(state, turn_taker, other_player);
}
