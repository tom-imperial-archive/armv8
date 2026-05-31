#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "utils/types.h"
#include "common/error.h"

/*
Returns a pointer to an array of uint32 representing *count instructions from the file at *path.
This pointer is guaranteed not to be NULL.
*/
uint32 *readfile(char *path, size_t *count) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        error(ERROR_READING_FILE, file_error_info(path));
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    if ((size % 4) != 0) {
        fclose(f);
        error(FILE_SIZE_NOT_MULTIPLE_OF_4, file_error_info(path));
        return NULL;
    }

    *count = size/4;
    uint32 *instrs = malloc(*count * sizeof(uint32));
    if (!instrs) {
        fclose(f);
        error(FAILED_TO_ALLOCATE, NULL);
        return NULL;
    }

    if (fread(instrs, sizeof(uint32), *count, f) != *count) {
        fclose(f);
        free(instrs);
        error(NOT_ALL_WORDS_READ, file_error_info(path));
        return NULL;
    }

    fclose(f);
    return instrs;

}

bool writefile(char *path, char *registers_out, char *nonzero_out) {
    FILE *f = fopen(path, "w");

    if (f == NULL) {
        error(ERROR_WRITING_FILE, file_error_info(path));
        return false;
    }

    fprintf(f, "%s\n%s\n", registers_out, nonzero_out);
    fclose(f);

    return true;
}

