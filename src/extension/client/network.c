/*
Checks for updates every frame, without interfering with main loop
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include "shared/log.h"
#include "shared/types.h"
#include "shared/protocol.h"

/* Establishes a non-blocking TCP connection to the game server
   Receives the server's address and the port the server is listening on.
   Returns the connected socket file descriptor. Exits if a failure occurs. */

int connect_to_server(char *hostname, int port) {
    struct sockaddr_in server_addr;
    struct hostent *server;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "%s\n", "[ERROR] Could not open socket for server connection");
        exit(EXIT_FAILURE);
    }

    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr, "[ERROR] No such host\n");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memmove(&server_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    server_addr.sin_port = htons(port);

    // Attempt to connect
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "%s\n", "[ERROR] Could not connect to server");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "%s\n", "[DEBUG] Connected to server");


    // Set socket to non-blocking, so recv() calls don't freeze the gui
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    return sockfd;
}
