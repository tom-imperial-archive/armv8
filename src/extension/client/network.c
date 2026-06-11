/*
Checks for updates every frame, without interfering with main loop
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
// TODO: FIX relative paths
#include "../shared/log.h"
#include "../shared/types.h"

void error(const char *msg) {
    perror(msg);
    printf("Something went wrong\n");
    exit(0);
}

void send_msg(int sockfd, char *msg) {
    int sent = send(sockfd, msg, strlen(msg), 0);

    if (sent < 0) {
        error("Error sending message");
    }

    printf("%s to server: %s", MESSAGE_SEND_SUCCESS, msg);
}

static int receive_int(int sockfd) {
    int msg;
    int n = read(sockfd, &msg, sizeof(int));

    if (n != sizeof(int)) {
        error("Error receiving int");
    }

    return msg;

}

void receive_msg(int sockfd, char **out)  {
    // server must send length before message
    uint32 length = (uint32) receive_int(sockfd);
    length = ntohl(length);

    char *msg = malloc(length+1);

    int n = read(sockfd, &msg, length);

    if (n < 0) {
        error("Error receiving message");
    }

    msg[length] = '\0';
    *out = msg;
}

int connect_to_server(char *hostname, int port) {
   
    struct sockaddr_in server_addr;
    struct hostent *server;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("Error opening socket for server connection");
    }

    server = gethostbyname(hostname);

    if (server == NULL) {
        fprintf(stderr, "ERROR: no such host\n");
        exit(0);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    memmove(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    server_addr.sin_port = htons(port);

    // attempt to connect
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        error("Error connecting to server");
    }

    printf(CONNECTION_SUCCESS_TO_SERVER);
    return(sockfd);
}