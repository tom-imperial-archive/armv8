#include "shared/protocol.h"
#include "shared/network.h"
#include "server/network.h"
#include "server/game.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void server_crash(void) {
    exit(EXIT_FAILURE);
}

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
        server_crash();
    }

    printf("Player 1 connected successfully!");

    if (player2_fd == -1) {
        fprintf(stderr, "[Error] Player 2 failed to connect!");
    }

    // Set up server state
    ServerState state = init_server_state();

    if (state == NULL) {
        server_crash();
    }

    send_packet(player1_fd, MSG_REQ_BOARD, NULL, 0);
    send_packet(player2_fd, MSG_REQ_BOARD, NULL, 0);

    // Set up players
    PlayerState p1 = new_player(player1_fd);
    PlayerState p2 = new_player(player2_fd);
    set_players(state, p1, p2);


    /*if (player1_fd != -1) {

        while (1) {
            PacketHeader header;
            void *payload = NULL;
            int status = receive_packet(player1_fd, &header, &payload);
            if (status == 1) {
                if (header.type == MSG_JOIN) {
                    printf("Success! Received MSG_JOIN from client!\n");
                }
                if (payload) { free(payload); }
                break;
            } else if (status == -1) {
                break; // Exit on error
            }

            usleep(10000); // To prevent killing my CPU
        }

        close (player1_fd);
    }
*/
    close(server_fd);
    return 0;
}
