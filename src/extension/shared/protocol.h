#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "types.h"
#include "board.h"

typedef enum {
    MSG_JOIN,
    MSG_BOARD_LAYOUT,
    MSG_FIRE,
    MSG_STATE_UPDATE,
    MSG_GAME_OVER
} MessageType;

// Universal header for every packet
typedef struct {
    MessageType type;
    uint32 payload_length;
} PacketHeader;

// Paylods for specific packets
typedef struct {
    uint8 x;
    uint8 y;
} FirePayload;

typedef struct {
    Board updated_board;
    bool is_your_turn;
} StateUpdatePayload;

typedef struct {
    bool you_won;
} GameOverPayload;

#endif
