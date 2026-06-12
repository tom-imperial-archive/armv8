#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "types.h"
#include "board.h"
#include <stdbool.h>

typedef enum {
    MSG_JOIN,
    MSG_REQ_BOARD,
    MSG_INIT_BOARD_LAYOUT,
    MSG_GAME_START,
    MSG_FIRE,
    MSG_RESULT,
    MSG_ATTACKED,
    MSG_GAME_OVER
} MessageType;

// Universal header for every packet
typedef struct {
    MessageType type;
    uint32 payload_length;
} PacketHeader;

// Payloads for specific packets
typedef struct {
    InitialShipDefs defs;
} InitBoardPayload;

typedef struct {
    uint8 x;
    uint8 y;
} FirePayload;

typedef struct {
    bool success;
    bool destroyed_ship;
    ShipType ship; // Only added if destroyed
} HitResultPayload;

typedef struct {
    FirePayload shot;
    HitResultPayload result;
} EnemyAttackPayload;

typedef struct {
    bool you_won;
} GameOverPayload;

typedef struct {
    bool your_turn;
} GameStartPayload;

#endif
