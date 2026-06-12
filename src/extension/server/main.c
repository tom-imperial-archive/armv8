#include "shared/protocol.h"
#include "shared/network.h"
#include "server/network.h"
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

    if (player1_fd != -1) {
        printf("Player 1 connected successfully!");

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

    close(server_fd);
    return 0;
}
