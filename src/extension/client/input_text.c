#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "input.h"

#define BUFFER_SIZE 10

InputData get_user_input(void) {
    InputData data;

    data.type = INPUT_NONE;
    data.grid_x = 0;
    data.grid_y = 0;

    char input[BUFFER_SIZE];
    if (fgets(input, BUFFER_SIZE, stdin) != NULL) {
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
