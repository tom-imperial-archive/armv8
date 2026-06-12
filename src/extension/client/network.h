#ifndef CLIENT_NETWORK_H
#define CLIENT_NETWORK_H

#include "shared/types.h"
#include "shared/protocol.h"

extern int connect_to_server(char *hostname, int port);

extern int send_packet(int sockfd, MessageType type, const void *payload, uint32 payload_length);

int receive_packet(int sockfd, PacketHeader *out_header, void **out_payload);

#endif
