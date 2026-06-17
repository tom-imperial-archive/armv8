#include "server/game.h"
#include "server/network.h"
#include "shared/network.h"
#include "shared/protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
Server executable:
    Connects to players, and boots up the state machine
*/

int main(void) {
    int port = 8080;
    int server_fd = start_server(port);

    printf("Waiting for Player 1...\n");
    int player1_fd = accept_client(server_fd);
    printf("Waiting for Player 2...\n");
    int player2_fd = accept_client(server_fd);

    if (player1_fd == -1) {
        fprintf(stderr, "[Error] Player 1 failed to connect!");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Player 1 connected successfully!\n");

    if (player2_fd == -1) {
        fprintf(stderr, "[Error] Player 2 failed to connect!");
        close(player1_fd);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Set up server state
    GameState state = init_game_state();

    if (state == NULL) {
        close(player1_fd);
        close(server_fd);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    send_packet(player1_fd, MSG_REQ_BOARD, NULL, 0);
    send_packet(player2_fd, MSG_REQ_BOARD, NULL, 0);

    // Set up players
    PlayerState p1 = new_player(player1_fd);
    PlayerState p2 = new_player(player2_fd);
    set_players(state, p1, p2);

    play(state);
    close_connections(state);
    free_game_state(state);

    close(server_fd);
    return 0;
}
