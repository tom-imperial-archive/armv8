#include "client/input.h"
#include "client/network.h"
#include "client/render.h"
#include "client/state.h"
#include "shared/network.h"
#include "shared/protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Returns true on success, false on failure
bool start_client_systems(ClientState *state, char *hostname, int port) {
    if (!init_graphics()) {
        fprintf(stderr, "%s\n", "[ERROR] Failed to initialise graphics engine");
        return false;
    }

    fprintf(stdout, "%s\n", "[DEBUG] Attempting to connect to server...\n");
    state->net.connection_fd = connect_to_server(hostname, port);

    if (state->net.connection_fd != -1) {
        fprintf(stdout, "%s\n", "[DEBUG] Connected successfully!");
        state->current_state = UI_STATE_PLACING_SHIPS;
        return true;
    } else {
        fprintf(stdout, "%s\n", "[ERROR] Failed to connect to server!");
        return false;
    }
}

static void send_board_to_server(ClientState *state) {
    InitBoardPayload payload;
    for (int i = 0; i < NUM_SHIPS; i++) {
        payload.defs[i] = state->placement.placements[i];
    }
    send_packet(state->net.connection_fd, MSG_INIT_BOARD_LAYOUT, &payload,
                sizeof(payload));
    fprintf(stdout, "%s\n", "[DEBUG] Sent board layout to server.");
}

static void handle_msq_req_board(ClientState *state) {
    state->net.server_requested_board = true;
    if (state->current_state == UI_STATE_WAITING_FOR_OPPONENT) {
        send_board_to_server(state);
        fprintf(stdout, "[DEBUG] Sent ship layout to server.\n");
    } else {
        fprintf(stdout, "[DEBUG] Server requested board, but we are still placing ships.\n");
    }
}

static void handle_msg_game_start(ClientState *state, GameStartPayload *start_data) {
    state->current_state =
            start_data->your_turn ? UI_STATE_MY_TURN : UI_STATE_OPPONENT_TURN;
    fprintf(stdout, "[DEBUG] Game has been started.\n");
}

static void handle_msg_attack(ClientState *state, EnemyAttackPayload *hit_data) {
    FirePayload shot = hit_data->shot;
    HitResultPayload result = hit_data->result;

    Board target = (state->current_state == UI_STATE_MY_TURN) ? state->game.target_board : state->game.my_board;
    UIState next_state = (state->current_state == UI_STATE_MY_TURN) ? UI_STATE_OPPONENT_TURN : UI_STATE_MY_TURN;

    board_mark_strike(target, (Position){.x = shot.x, .y = shot.y}, result.success);

    // Now check if the shot sank a ship
    if (result.ship != -1) {
        board_mark_sunk_ship(target, result.ship, result.sunk_pos);
        if (state->current_state == UI_STATE_MY_TURN) {
            state->game.enemy_ships_sunk[result.ship] = true;
            state->game.enemy_ship_positions[result.ship] = result.sunk_pos;
        }

        fprintf(stdout, "%s\n", "[DEBUG] Ship sunk!");
    } else {
        fprintf(stdout, "%s\n", "[DEBUG] Marked the hit.");
    }

    state->current_state = next_state;
}

static void handle_msg_game_over(ClientState *state, GameOverPayload *result_data) {
    state->is_running = false;
    state->game.i_won = result_data->you_won;

    char *winner = state->game.i_won ? "You" : "Your opponent";
    fprintf(stdout, "[DEBUG] %s won!\n", winner);
}


static void handle_incoming_packet(ClientState *state, PacketHeader header,
                                   void *payload) {
    switch (header.type) {
    case MSG_REQ_BOARD:
        handle_msq_req_board(state);
        break;
    case MSG_GAME_START:
        handle_msg_game_start(state, (GameStartPayload *)payload);
        break;
    case MSG_ATTACKED:
        handle_msg_attack(state, (EnemyAttackPayload *)payload);
        break;
    case MSG_GAME_OVER: 
        // This is also called unexpectedly in the case where the opponent
        // resigned or similar
        handle_msg_game_over(state, (GameOverPayload *)payload);
        break;
    case MSG_INVALID_BOARD:
        fprintf(stdout, "[ERROR] Invalid placement: ships overlap "
                            "or are out of bounds!\n");

        reset_staged_ships();

        free_board(state->game.my_board);
        state->game.my_board = create_empty_board();    
        break;
    default:
        fprintf(stderr, "%s\n", "[ERROR] Unexpected packet type received");
    }
}

static void hardcode_ship_placement(ClientState *state) {
    InitialShipDefs my_ships = {
        {SHIP_BATTLESHIP,
         {1, 1, true}}, // Length 5: Horizontal at (1,1) -> covers X: 1 to 5
        {SHIP_CARRIER,
         {8, 2, false}}, // Length 4: Vertical at (8,2) -> covers Y: 2 to 5
        {SHIP_CRUISER,
         {2, 4, true}}, // Length 3: Horizontal at (2,4) -> covers X: 2 to 4
        {SHIP_SUBMARINE,
         {5, 6, false}}, // Length 3: Vertical at (5,6) -> covers Y: 6 to 8
        {SHIP_DESTROYER,
         {0, 8, true}} // Length 2: Horizontal at (0,8) -> covers X: 0 to 1
    };

    // Populate underlying logic board
    if (!board_add_placement_set(my_ships, state->game.my_board)) {
        fprintf(stderr, "[ERROR] Hardcoded ship placement was invalid!\n");
        return;
    }

    // Copy to placement buffer
    for (int i = 0; i < NUM_SHIPS; i++) {
        state->placement.placements[i] = my_ships[i];
    }

    // Switch to next state
    state->current_state = UI_STATE_WAITING_FOR_OPPONENT;

    fprintf(stdout, "[DEBUG] Auto-placed ships, yet to send layout.\n");
}

static void handle_state_placing_ships(ClientState *state, InputData input) {
    // HARDCODED VERSION - FOR DEBUGGING
    bool hardcode_ships = false;
    if (hardcode_ships) {
        hardcode_ship_placement(state);
        return;
    }
    // ACTUAL VERSION
    // Attempt to place
    if (input.type == INPUT_PLACED_SHIPS) {
        if (board_add_placement_set(input.ships, state->game.my_board)) {
            // Success
            for (int i = 0; i < NUM_SHIPS; i++) {
                state->placement.placements[i] = input.ships[i];
            }

            state->current_state = UI_STATE_WAITING_FOR_OPPONENT;
            fprintf(stdout, "[DEBUG] Ships placed successfully! "
                            "Waiting for opponent...\n");

            if (state->net.server_requested_board) {
                send_board_to_server(state);
            }
        } else {
            // Invalid!
            fprintf(stdout, "[ERROR] Invalid placement: ships overlap "
                            "or are out of bounds!\n");

            reset_staged_ships();

            free_board(state->game.my_board);
            state->game.my_board = create_empty_board();
        }
    }
}

static void handle_state_my_turn(ClientState *state, InputData input) {
    // Check if we got an input
    if (input.type == INPUT_FIRE) {
        if (valid_attack_pos(state->game.target_board,
                                (Position){.x = input.grid_pos.x,
                                        .y = input.grid_pos.y})) {
            FirePayload fire_req;
            fire_req.x = input.grid_pos.x;
            fire_req.y = input.grid_pos.y;

            send_packet(state->net.connection_fd, MSG_FIRE, &fire_req,
                        sizeof(fire_req));
            fprintf(stdout, "[DEBUG] Fired at %d, %d!\n",
                    input.grid_pos.x, input.grid_pos.y);
        } else {
            fprintf(stdout, "[DEBUG] Invalid coordinate!\n");
        }
    }
}

void client_loop(ClientState *state) {
    while (state->is_running && is_window_open()) {
        // Receive data from server
        PacketHeader header;
        void *payload = NULL;
        int recv_status;
        while ((recv_status = receive_packet(state->net.connection_fd, &header,
                                             &payload)) == 1) {
            handle_incoming_packet(state, header, payload);
            if (payload) {
                free(payload);
            }
        }

        // Check for crash/disconnect
        if (recv_status == -1) {
            fprintf(stderr,
                    "\n[ERROR] Lost connection to the server! Exiting...\n");
            state->is_running = false;
            break; // Break out of the game loop immediately
        }

        // Receive input
        InputData input = get_user_input();
        if (input.type == INPUT_QUIT) {
            state->is_running = false;
        }

        switch (state->current_state) {
            case UI_STATE_PLACING_SHIPS:
                handle_state_placing_ships(state, input);
                break;
            case UI_STATE_MY_TURN:
                handle_state_my_turn(state, input);
                break;
            case UI_STATE_GAME_OVER:
                // DONT THINK WE NEED ANYTHING HERE - BUT CHECK
            case UI_STATE_OPPONENT_TURN:
            case UI_STATE_WAITING_FOR_OPPONENT:
            case UI_STATE_CONNECTING:
                // Do nothing, just sit tight
                break;
        }

        render_frame(state);

        usleep(16000);
    }
}

void client_cleanup(ClientState *state) {
    if (state->net.connection_fd != -1) {
        close(state->net.connection_fd);
    }

    free_client_state(state);
    cleanup_graphics();
    fprintf(stdout, "%s\n", "[DEBUG] Client shut down cleanly.");
}
