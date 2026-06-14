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
bool start_client_systems(ClientState *state, char *hostname) {
    if (!init_graphics()) {
        fprintf(stderr, "%s\n", "[ERROR] Failed to initialise graphics engine");
        return false;
    }

    fprintf(stdout, "%s\n", "[DEBUG] Attempting to connect to server...\n");
    state->connection_fd = connect_to_server(hostname, 8080);

    if (state->connection_fd != -1) {
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
        payload.defs[i] = state->my_placements[i];
    }
    send_packet(state->connection_fd, MSG_INIT_BOARD_LAYOUT, &payload,
                sizeof(payload));
    fprintf(stdout, "%s\n", "[DEBUG] Sent board layout to server.");
}

static void handle_incoming_packet(ClientState *state, PacketHeader header,
                                   void *payload) {
    switch (header.type) {
    case MSG_REQ_BOARD:
        state->server_requested_board = true;

        if (state->current_state == UI_STATE_WAITING_FOR_OPPONENT) {
            send_board_to_server(state);
            fprintf(stdout, "[DEBUG] Sent ship layout to server.\n");
        } else {
            fprintf(stdout, "[DEBUG] Server requested board, but we are still placing ships.\n");
        }
        break;
    case MSG_GAME_START: {
        GameStartPayload *start_data = (GameStartPayload *)payload;
        state->current_state =
            start_data->your_turn ? UI_STATE_MY_TURN : UI_STATE_OPPONENT_TURN;
        fprintf(stdout, "[DEBUG] Game has been started.\n");
        break;
    }
    case MSG_ATTACKED:
        if (state->current_state == UI_STATE_MY_TURN) {
            // THIS WILL BE RENAMED
            EnemyAttackPayload *hit_data = (EnemyAttackPayload *)payload;
            FirePayload shot = hit_data->shot;
            HitResultPayload result = hit_data->result;

            board_mark_strike(state->target_board,
                              (Position){.x = shot.x, .y = shot.y},
                              result.success);
            fprintf(stdout, "[DEBUG] Marked the hit on TARGET board.\n");
            // DO SOME MAGIC ABOUT DESTROY SHIP

            // SWAP TURN
            state->current_state = UI_STATE_OPPONENT_TURN;
        } else if (state->current_state == UI_STATE_OPPONENT_TURN) {
            // DO SAME THING JUST ON THE OTHER BOARD

            // THIS WILL BE RENAMED
            EnemyAttackPayload *hit_data = (EnemyAttackPayload *)payload;
            FirePayload shot = hit_data->shot;
            HitResultPayload result = hit_data->result;

            board_mark_strike(state->my_board,
                              (Position){.x = shot.x, .y = shot.y},
                              result.success);
            fprintf(stdout, "[DEBUG] Marked the hit on MY board.\n");
            // SWAP TURN
            state->current_state = UI_STATE_MY_TURN;
        }
        break;
    case MSG_GAME_OVER: {
        // HANDLE GAME OVER
        // This is also called unexpectedly in the case where the opponent
        // resigned or similar
        GameOverPayload *game_over = (GameOverPayload *)payload;
        state->is_running = false;
        state->i_won = game_over->you_won;
        char *winner = state->i_won ? "You" : "Your opponent";
        fprintf(stdout, "[DEBUG] %s won!\n", winner);
    }
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
    if (!board_add_placement_set(my_ships, state->my_board)) {
        fprintf(stderr, "[ERROR] Hardcoded ship placement was invalid!\n");
        return;
    }

    // Copy to placement buffer
    for (int i = 0; i < NUM_SHIPS; i++) {
        state->my_placements[i] = my_ships[i];
    }

    // Switch to next state
    state->current_state = UI_STATE_WAITING_FOR_OPPONENT;

    fprintf(stdout, "[DEBUG] Auto-placed ships, yet to send layout.\n");
}

void client_loop(ClientState *state) {
    while (state->is_running && is_window_open()) {
        // Receive data from server
        PacketHeader header;
        void *payload = NULL;
        int recv_status;
        while ((recv_status = receive_packet(state->connection_fd, &header,
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
            // HARDCODED VERSION
            // hardcode_ship_placement(state);

            // ACTUAL VERSION
            // Attempt to place
            if (input.type == INPUT_PLACED_SHIPS) {
                if (board_add_placement_set(input.ships, state->my_board)) {
                    // Success
                    for (int i = 0; i < NUM_SHIPS; i++) {
                        state->my_placements[i] = input.ships[i];
                    }

                    state->current_state = UI_STATE_WAITING_FOR_OPPONENT;
                    fprintf(stdout, "[DEBUG] Ships placed successfully! "
                                    "Waiting for opponent...\n");

                    if (state->server_requested_board) {
                        send_board_to_server(state);
                    }
                } else {
                    // Invalid!
                    fprintf(stdout, "[ERROR] Invalid placement: ships overlap "
                                    "or are out of bounds!\n");

                    reset_staged_ships();

                    free_board(state->my_board);
                    state->my_board = create_empty_board();
                }
            }
            break;
        case UI_STATE_MY_TURN:
            // Check if we got an input
            if (input.type == INPUT_FIRE) {
                if (valid_attack_pos(state->target_board,
                                     (Position){.x = input.grid_pos.x,
                                                .y = input.grid_pos.y})) {
                    FirePayload fire_req;
                    fire_req.x = input.grid_pos.x;
                    fire_req.y = input.grid_pos.y;

                    send_packet(state->connection_fd, MSG_FIRE, &fire_req,
                                sizeof(fire_req));
                    fprintf(stdout, "[DEBUG] Fired at %d, %d!\n",
                            input.grid_pos.x, input.grid_pos.y);
                } else {
                    fprintf(stdout, "[DEBUG] Invalid coordinate!\n");
                }
            }
            break;
        case UI_STATE_GAME_OVER:
            // DONT THINK WE NEED ANYTHING HERE
            break;
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
    if (state->connection_fd != -1) {
        close(state->connection_fd);
    }

    free_client_state(state);
    cleanup_graphics();
    fprintf(stdout, "%s\n", "[DEBUG] Client shut down cleanly.");
}
