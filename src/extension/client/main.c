#include "shared/protocol.h"
#include "shared/network.h"
#include "client/network.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/*
Entry point for client executable
Initialises local state storage and runs the game loop
*/

int main() {
    printf("Attempting to connect to server...\n");
    // Currently in local host, but can work with any DoC machine
    // e.g. gpu17.doc.ic.ac.uk
    int sockfd = connect_to_server("127.0.0.1", 8080);

    if (sockfd >= 0) {
        printf("Connected! Sending MSG_JOIN packet...\n");

        // Send a packet with no payload just to test the connection
        if (send_packet(sockfd, MSG_JOIN, NULL, 0) == 0) {
            printf("SUCCESS: Packet sent.\n");
        }

        // Wait a moment so the server has time to process before we close
        sleep(1);
        close(sockfd);
    }

    return 0;
}
