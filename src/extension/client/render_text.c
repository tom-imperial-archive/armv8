#include <stdio.h>
#include <stdbool.h>
#include "render.h"

bool init_graphics(void) {
    printf("=== Initialising ===\n");
    return true;
}

bool is_window_open(void) {
    return true;
}

static UIState prev_state = -1; // -1 forces a draw on the very first frame
static int prev_ships_placed = -1;

void render_frame(const ClientState *state) {
    if (state->current_state == prev_state &&
        state->ships_placed == prev_ships_placed) {
        return;
    }

    prev_state = state->current_state;
    prev_ships_placed = state->ships_placed;

    //printf("\033[H"); // clear terminal
    switch (state->current_state) {
        default:
        case UI_STATE_CONNECTING:
            printf("=== Connecting to server ===\n");
            break;
        case UI_STATE_WAITING_FOR_OPPONENT:
            printf("=== Waiting for opponent to join ===\n");
            break;
        case UI_STATE_MY_TURN:
            printf("=== YOUR TURN ===\n");
            printf("=== Your board ===\n");
            print_board(state->my_board, stdout);
            printf("\n=== Target board ===\n");
            print_board(state->target_board, stdout);
            break;
        case UI_STATE_OPPONENT_TURN:
            printf("=== OPPONENT'S TURN ===\n");
            printf("=== Your board ===\n");
            print_board(state->my_board, stdout);
            printf("\n=== Target board ===\n");
            print_board(state->target_board, stdout);
            break;
        case UI_STATE_GAME_OVER:
            if (state->i_won) {
                printf("=== YOU WON!! ===\n");
            } else {
                printf("=== LOSER!! ==\n");
            }
            break;
    }
}

void cleanup_graphics(void) {
    return;
}
