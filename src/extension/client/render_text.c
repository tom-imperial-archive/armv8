#include <stdio.h>
#include "render.h"

bool init_graphics(void) {
    printf("=== Initialising ===\n");
    return 1;
}

bool is_window_open(void) {
    return 1;
}

void render_frame(const ClientState *state) {
    printf("\033[H\033[J"); // clear terminal
    switch (state->current_state) {
        default:
        case UI_STATE_CONNECTING:
            printf("=== Connecting to server ===\n");
        case UI_STATE_WAITING_FOR_OPPONENT:
            printf("=== Waiting for opponent to join ===\n");
        case UI_STATE_MY_TURN:
            printf("=== Your board ===\n");
            print_board(state->my_board, stdout);
            printf("\n=== Target board ===\n");
            print_board(state->target_board, stdout);
        case UI_STATE_OPPONENT_TURN:
            printf("=== Waiting for opponent to play ===\n");
        case UI_STATE_GAME_OVER:
            if (state->i_won) {
                printf("=== YOU WON!! ===\n");
            } else {
                printf("=== LOSER!! ==\n");
            }
    }
}

void cleanup_graphics(void) {
    return;
}