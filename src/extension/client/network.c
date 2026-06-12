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
        error("Error opening socket for server connection");
    }

    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr, "ERROR: no such host\n");
        exit(0);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memmove(&server_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    server_addr.sin_port = htons(port);

    // Attempt to connect
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        error("Error connecting to server");
    }

    fprintf(stdout, "%s\n", "[DEBUG] Connected to server");


    // Set socket to non-blocking, so recv() calls don't freeze the gui
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    return sockfd;
}


/* Sends a packet header and optional payload to the server.
   Receives the connected socket file descriptor, the MessageType enum,
   a pointer to the payload being sent (which can be NULL) and the
   payload lenght in bytes.
   Returns 0 on successful transmission, -1 on failure. */
int send_packet(int sockfd, MessageType type, const void *payload, uint32 payload_length) {
    PacketHeader header;
    header.type = type;

    // Convert the integer payload length to Network Byte Order before sending
    header.payload_length = htonl(payload_length);

    // Send the header
    int sent = send(sockfd, &header, sizeof(PacketHeader), 0);
    if (sent < 0) {
        fprintf(stdout, "%s\n", "[ERROR] Failed to send packet header");
        return -1;
    }

    // Send the payload, if there is one
    if (payload_length > 0 && payload != NULL) {
        sent = send(sockfd, payload, payload_length, 0);
        if (sent < 0) {
            fprintf(stderr, "%s\n", "[ERROR] Failed to send packet payload");
            return -1;
        }
    }

    fprintf(stdout, "[DEBUG] Sent packet (Type: %d, Payload Size: %u bytes)\n", type, payload_length);
    return 0;
}

/* Non-blocking poll to read incoming packets from the server. Designed to
   be called once per frame. If a payload is received, then the memory is
   dynamically allocated, and it is the caller's responsibility to free it.
   Receives the socket file descriptor, a pointer to a local PacketHeader to
   populate, and an address to a void pointer which will be set to newly
   allocated memory containing the payload (if any).
   Returns 1 if a packet was caught, 0 if nothing, and -1 on
   fatal error or disconnect. */

int receive_packet(int sockfd, PacketHeader *out_header, void **out_payload) {
    // Attempt to read the header
    int n = recv(sockfd, out_header, sizeof(PacketHeader), 0);

    if (n < 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            // No data yet, but nothing went wrong
            return 0;
        }
        // If we got some other error, then that's a genuine error
        fprintf(stderr, "%s\n", "[ERROR] Socket error during receive");
    } else if (n == 0) {
        fprintf(stderr, "%s\n", "[ERROR] Server closed the connection");
    }

    // Now we have a header
    out_header->payload_length = ntohl(out_header->payload_length);

    // Read the payload if we are expecting one
    if (out_header->payload_length > 0) {
        *out_payload = malloc(out_header->payload_length);

        if (*out_payload == NULL) {
            fprintf(stderr, "%s\n", "[ERROR] Memory allocation faield for payload");
            return -1;
        }

        // Read the entire payload
        int total_read = 0;
        while (total_read < out_header->payload_length) {
            int payload_bytes = recv(sockfd, (char*)*out_payload + total_read, out_header->payload_length - total_read, 0);
            if (payload_bytes  > 0) {
                total_read += payload_bytes;
            } else if (payload_bytes < 0 && errno != EWOULDBLOCK && errno != EAGAIN) {
                fprintf(stderr, "%s\n", "[ERROR] Failed to read complete payload");
                return -1;
            }
        }
    } else {
        // Some packets have no payload
        *out_payload = NULL;
    }

    fprintf(stdout, "[DEBUG] Recieved packet (Type: %d, Payload Size: %u bytes)\n", out_header->type, out_header->payload_length);
    return 1;
}
