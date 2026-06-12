#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>
#include "input.h"

#define BUFFER_SIZE 10

static int input_available(void) {
    struct timeval tv = {0, 0};  // zero timeout = don't wait at all
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
}

InputData get_user_input(void) {
    InputData data;

    data.type = INPUT_NONE;
    data.grid_x = 0;
    data.grid_y = 0;

    if (!input_available()) {
        return data; // INPUT_NONE
    }

    char input[BUFFER_SIZE];
    if (fgets(input, BUFFER_SIZE, stdin) != NULL) {
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "quit") == 0) {
            data.type = INPUT_QUIT;
        } else {
            // expect grid coords
            data.type = INPUT_SELECT_GRID;
            char *start = input;
            while (isspace(*start)) start++;
            // expect digit
            if (isdigit(*start) && atoi(start) < 11) {
                data.grid_x = atoi(start);
                while (isdigit(*start)) start++;
            } else {
                data.type = INPUT_NONE;
            }

            while (isspace(*start)) start++;
            // expect digit
            if (isdigit(*start) && atoi(start) < 11) {
                data.grid_y = atoi(start);
            } else {
                data.type = INPUT_NONE;
            }
        }
    }

    return data;
}
