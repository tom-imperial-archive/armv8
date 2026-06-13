#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

// Type aliases
typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;

typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;

// Global constants
#define BOARD_SIZE 10
#define NUM_SHIPS 5
#define MAX_USERNAME_LEN 16

// Universal data structures
typedef enum {
    DIR_HORIZONTAL = 0,
    DIR_VERTICAL = 1
} Orientation;

typedef enum {
    SHIP_BATTLESHIP,
    SHIP_CARRIER,
    SHIP_CRUISER,
    SHIP_SUBMARINE,
    SHIP_DESTROYER
} ShipType;

typedef struct {
    int8 x;
    int8 y;
} Coordinate;

#endif
