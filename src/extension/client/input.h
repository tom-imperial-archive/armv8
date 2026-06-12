/*
Universal interface for output
*/

#ifndef CLIENT_INPUT_H
#define CLIENT_INPUT_H

#include <stdbool.h>

typedef enum {
    INPUT_NONE,
    INPUT_QUIT,
    INPUT_SELECT_GRID,
    INPUT_ROTATE_SHIP,
    INPUT_CONFIRM
} InputType;

typedef struct {
    InputType type;
    // Optional coordinates
    int grid_x;
    int grid_y;
} InputData;

// Receives input, must be non-blocking.
// If no input, return { INPUT_NONE, 0, 0 }
extern InputData get_user_input(void);

#endif
